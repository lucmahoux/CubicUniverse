#include "chunk_utils.h"

// ----------------------------------------------------------------------------

/* A simple recursive DFS procedure to perform a flood fill algorithm
 * starting from 'org_pos'.
 * Only the non-opaque blocks allow the flood fill to further expand.
 * The output is the variable 'connected_faces' which shall be equal to 0
 * upon calling. This variable is updated with the bitmasks of the faces
 * reachable by the flood fill. */
static void visibility_graph_dfs(SubChunk* sc, GraphMarker* block_marker,
                          SubChunkCoords org_pos, uint8_t* connected_faces);

/* Simply updates the sc->visibility_graph with regard to the flood fill results */
static void process_visibility_dfs_output(SubChunk* sc, uint8_t connected_faces);

// ----------------------------------------------------------------------------

SubChunkCoords chunk_coords_to_subchunk(ChunkCoords c_coords) {
    SubChunkCoords sc_coords = {
        .x = c_coords.x,
        .y = c_coords.y % 16,
        .z = c_coords.z
    };
    return sc_coords;
}

ChunkCoords subchunk_coords_to_chunk(SubChunk* sc, SubChunkCoords sc_coords) {
    ChunkCoords c_coords = {
        .x = sc_coords.x,
        .y = (sc->y_pos << 4) + sc_coords.y,
        .z = sc_coords.z
    };
    return c_coords;
}

vec3 chunk_coords_to_world(Chunk* chunk, ChunkCoords c_coords) {
    return VEC3(1.0f * (chunk->chunkX * 16) + c_coords.x,
                1.0f * c_coords.y,
                1.0f * (chunk->chunkZ * 16) + c_coords.z);
}

vec3 subchunk_coords_to_world(Chunk* chunk, SubChunk* sc,
                              SubChunkCoords sc_coords) {
    return VEC3(1.0f * (chunk->chunkX * 16) + sc_coords.x,
                1.0f * ((sc->y_pos * 16) + sc_coords.y),
                1.0f * (chunk->chunkZ * 16) + sc_coords.z);
}

ChunkCoords world_coords_to_chunk(vec3 w_coords) {
    int x = (int)((long)(w_coords.coords[0]) % 16);
    int z = (int)((long)(w_coords.coords[2]) % 16);
    ChunkCoords c_coords = {
        .x = (uint8_t)(x < 0 ? (x + 16) : x),
        .y = (uint8_t)w_coords.coords[1],
        .z = (uint8_t)(z < 0 ? (z + 16) : z)
    };
    return c_coords;
}

void chunk_init(Chunk* chunk, int32_t chunkX, int32_t chunkZ) {
    chunk->chunkX = chunkX;
    chunk->chunkZ = chunkZ;
    chunk->nb_subchunks = 0;
    chunk->flags.is_modified = false;
    chunk->flags.is_loaded = true;
    for (uint8_t i = 0; i < MAX_SUBCHUNKS; ++i) {
        SubChunk* SC = chunk->subchunks + i;
        SC->y_pos = MAX_SUBCHUNKS;
        SC->palette.BP_elts = NULL;
        SC->palette.BS_uid_mapper = NULL;
        SC->blocks = NULL;
        memset(SC->visibility_graph, 0x3f, 6 * sizeof(uint8_t));
        SC->visibility_graph[FID_ENTRANCES] = 0;
    }
}

BP_elt* subchunk_get_BP_elt(SubChunk* SC, SubChunkCoords position) {
    palette_id bp_uid = *CUB_SUBCHUNK_BP_UID(SC, position);
    return bp_uid != AIR_ID
           ? *(BP_elt**)list_get(SC->palette.BP_elts, bp_uid - 1)
           : NULL;
}

#define VISIBILITY_GRAPH_NEIGHBOR_CHECK(COORDINATE, upper_bound, \
                                        faceID, opposite_faceID) \
    if (org_pos.COORDINATE > 0) {\
        org_pos.COORDINATE -= 1;\
        neighbor = subchunk_get_BP_elt(sc, org_pos);\
        neighbor_id = CUB_SUBCHUNK_BLOCK_ID(sc, org_pos);\
        if (!utils_graph_marker_is_marked(block_marker, neighbor_id)\
            && (!neighbor || !neighbor->BS.block->flags.is_opaque))\
            visibility_graph_dfs(sc, block_marker, org_pos, connected_faces);\
        org_pos.COORDINATE += 1;\
    } else *connected_faces |= FACE_BITMASK(faceID);\
    if (org_pos.COORDINATE < upper_bound - 1) {\
        org_pos.COORDINATE += 1;\
        neighbor = subchunk_get_BP_elt(sc, org_pos);\
        neighbor_id = CUB_SUBCHUNK_BLOCK_ID(sc, org_pos);\
        if (!utils_graph_marker_is_marked(block_marker, neighbor_id)\
            && (!neighbor || !neighbor->BS.block->flags.is_opaque))\
            visibility_graph_dfs(sc, block_marker, org_pos, connected_faces);\
        org_pos.COORDINATE -= 1;\
    } else *connected_faces |= FACE_BITMASK(opposite_faceID);

static void visibility_graph_dfs(SubChunk* sc, GraphMarker* block_marker,
                          SubChunkCoords org_pos, uint8_t* connected_faces) {
    utils_graph_marker_mark(block_marker, CUB_SUBCHUNK_BLOCK_ID(sc, org_pos));
    uint16_t neighbor_id;
    BP_elt* neighbor;
    VISIBILITY_GRAPH_NEIGHBOR_CHECK(x, CHUNK_WIDTH, FID_LEFT, FID_RIGHT)
    VISIBILITY_GRAPH_NEIGHBOR_CHECK(y, SUBCHUNK_HEIGHT, FID_BOTTOM, FID_TOP)
    VISIBILITY_GRAPH_NEIGHBOR_CHECK(z, CHUNK_DEPTH, FID_BACK, FID_FRONT)
}

static void process_visibility_dfs_output(SubChunk* sc, uint8_t connected_faces) {
    for (uint8_t faceID = FID_START; (uint8_t) faceID <= FID_END; ++faceID)
        if (connected_faces & FACE_BITMASK(faceID))
            sc->visibility_graph[faceID] |= connected_faces;
}

#define VISIBILITY_GRAPH_BUILDING_ROUTINE \
    org = subchunk_get_BP_elt(sc, org_pos);\
    org_id = CUB_SUBCHUNK_BLOCK_ID(sc, org_pos);\
    if (!utils_graph_marker_is_marked(block_marker, org_id)\
        && (!org || !org->BS.block->flags.is_opaque)) {\
        connected_faces = 0;\
        visibility_graph_dfs(sc, block_marker, org_pos, &connected_faces);\
        process_visibility_dfs_output(sc, connected_faces);\
    }

void subchunk_build_visibility_graph(SubChunk* sc, GraphMarker* block_marker) {
    utils_graph_marker_init(block_marker);
    memset(sc->visibility_graph, 0, 6 * sizeof(uint8_t));
    SubChunkCoords org_pos = { .x = 0, .y = 0, .z = 0 };
    uint16_t org_id = 0;
    BP_elt* org;
    uint8_t connected_faces;
    for (uint8_t i = 0; i < CHUNK_WIDTH; ++i) {
        for (uint8_t j = 0; j < CHUNK_DEPTH; ++j) {
            org_pos.x = i; org_pos.y = j; org_pos.z = 0;
            VISIBILITY_GRAPH_BUILDING_ROUTINE
            org_pos.z = CHUNK_DEPTH - 1;
            VISIBILITY_GRAPH_BUILDING_ROUTINE
            org_pos.x = i; org_pos.y = 0; org_pos.z = j;
            VISIBILITY_GRAPH_BUILDING_ROUTINE
            org_pos.y = SUBCHUNK_HEIGHT - 1;
            VISIBILITY_GRAPH_BUILDING_ROUTINE
            org_pos.x = 0; org_pos.y = i; org_pos.z = j;
            VISIBILITY_GRAPH_BUILDING_ROUTINE
            org_pos.x = CHUNK_WIDTH - 1;
            VISIBILITY_GRAPH_BUILDING_ROUTINE
        }
    }

    printf("0x%x - 0x%x - 0x%x - 0x%x - 0x%x - 0x%x\n",
            sc->visibility_graph[FID_TOP], sc->visibility_graph[FID_BOTTOM],
            sc->visibility_graph[FID_LEFT], sc->visibility_graph[FID_RIGHT],
            sc->visibility_graph[FID_FRONT], sc->visibility_graph[FID_BACK]);
}

void subchunk_free(SubChunk* SC) {
    printf("Subchunk_free: y_pos = %u - palette->len = %lu\n",
            SC->y_pos, SC->palette.BS_uid_mapper->nb_keys);
    BP_elt** BP_elts = SC->palette.BP_elts->data;
    for (size_t i = 0; i < SC->palette.BP_elts->len; ++i)
        if (BP_elts[i] != NULL)
            block_BP_elt_free(BP_elts[i]);
    hashmap_free(SC->palette.BS_uid_mapper);
    list_free(SC->palette.BP_elts);
    free(SC->blocks);
}

void chunk_free(Chunk* chunk) {
    printf("Chunk free (chunkX = %i - chunkZ = %i)\n",
            chunk->chunkX, chunk->chunkZ);
    for (size_t sc_id = 0; sc_id < MAX_SUBCHUNKS; ++sc_id)
        if (chunk->subchunks[sc_id].y_pos < MAX_SUBCHUNKS)
            subchunk_free(chunk->subchunks + sc_id);
}
