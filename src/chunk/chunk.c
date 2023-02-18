#include "chunk.h"

// ----------------------------------------------------------------------------
/* Called to initialise *subchunk and its fields (malloc, ...).
 * [!] The visibility is NOT updated in any way... [!] */
void subchunk_initialise(SubChunk* SC, uint8_t y_pos);

/* Tries to add 'BS' to the palette (it must not be an heap allocation).
 * If it already exists, adds 1 to the block counter, otherwise create the
 * 'bs' via a heap allocation & add it to the palette.
 * Returns the bs UID in the palette. */
palette_id subchunk_palette_add(SubChunk* SC, BlockState* BS);
// ----------------------------------------------------------------------------

void subchunk_initialise(SubChunk* SC, uint8_t y_pos) {
    SC->palette.BS_uid_mapper =
        hashmap_new(PALETTE_DEFAULT_LEN, PALETTE_DEFAULT_HASH_NBR);
    SC->palette.BP_elts = list_new(sizeof(BP_elt*), REALLOC_INCREMENT,
                                   PALETTE_DEFAULT_LEN);
    SC->y_pos = y_pos;
    SC->blocks = malloc(4096 * sizeof(palette_id));
    if (!SC->blocks)
        errx(EXIT_FAILURE, "subchunk_create: Malloc failed!");
    memset(SC->blocks, AIR_ID, 4096 * sizeof(palette_id));
}

palette_id subchunk_palette_add(SubChunk* SC, BlockState* BS) {
    palette_id bp_uid = (palette_id)(size_t)
                        hashmap_get(SC->palette.BS_uid_mapper, BS->BS_uid);
    BP_elt** BP_elts = SC->palette.BP_elts->data;
    if (bp_uid) { // BlockState already exists in palette
        ++BP_elts[bp_uid - 1]->nb_blocks;
    } else { // Create a new BP_elt -> default nb_blocks is already 1
        for (bp_uid = 0; bp_uid < SC->palette.BP_elts->len; ++bp_uid)
            if (BP_elts[bp_uid] == NULL)
                break;
        BP_elt* new_elt = block_BP_elt_new(BS);
        if (bp_uid >= SC->palette.BP_elts->len)
            list_append(SC->palette.BP_elts, &new_elt);
        else
            BP_elts[bp_uid] = new_elt;
        hashmap_set(SC->palette.BS_uid_mapper, BS->BS_uid,
                    (void*)(size_t)(++bp_uid));
    }
    return bp_uid;
}

void subchunk_palette_remove_block(GraphMarker* block_marker,
                                   SubChunk* SC, palette_id* bp_uid) {
    const BP_elt* NULL_elt = NULL;
    BP_elt* BP_elt = list_get(SC->palette.BP_elts, *bp_uid - 1);
    *bp_uid = AIR_ID;
    if (BP_elt->BS.block->flags.is_opaque)
        subchunk_build_visibility_graph(SC, block_marker);
    if (--BP_elt->nb_blocks == 0) { // Remove the BP_elt
        list_set(SC->palette.BP_elts, &NULL_elt, (size_t)(*bp_uid - 1));
        hashmap_remove(SC->palette.BS_uid_mapper, BP_elt->BS.BS_uid);
        block_BP_elt_free(BP_elt);
    }
}

void chunk_set_block(GraphMarker* block_marker, Chunk* chunk,
                     ChunkCoords c_coords, BlockState* BS) {
    SubChunk* SC = chunk->subchunks + (c_coords.y >> 4);
    if (BS->block->id == AIR_ID) { // Block is destroyed (= replaced by air)
        if (SC->y_pos >= MAX_SUBCHUNKS)
            return; // Already empty subchunk (full of air)
        palette_id* bp_uid = CUB_CHUNK_BP_UID(SC, c_coords);
        if (*bp_uid != AIR_ID) // Not air => update block counter
            subchunk_palette_remove_block(block_marker, SC, bp_uid);
    } else if (SC->y_pos >= MAX_SUBCHUNKS) { // New subchunk -> Add block
        subchunk_initialise(SC, c_coords.y >> 4);
        palette_id bp_uid = subchunk_palette_add(SC, BS);
        *CUB_CHUNK_BP_UID(SC, c_coords) = bp_uid;
        if (BS->block->flags.is_opaque)
            subchunk_build_visibility_graph(SC, block_marker);
    } else { // Not empty block & already existing subchunk -> Add block
        palette_id* bp_uid = CUB_CHUNK_BP_UID(SC, c_coords);
        if (*bp_uid != AIR_ID) // Not air => update block counter
            subchunk_palette_remove_block(block_marker, SC, bp_uid);
        *bp_uid = subchunk_palette_add(SC, BS);
        if (BS->block->flags.is_opaque)
            subchunk_build_visibility_graph(SC, block_marker);
    }
    chunk->flags.is_modified = true;
}

/*void chunk_render(chunk* chunk, cub_unused blockRenderer* renderer, cub_unused camera* camera,
                  blockList* block_list) {
    cub_unused subChunkCoords sc_coords;
    cub_unused chunkCoords c_coords;
    for (uint8_t i = 0; i < MAX_SUBCHUNKS; ++i) {
        subChunk* sc = chunk->subchunks + i;
        if (sc->y_pos >= MAX_SUBCHUNKS) continue;
        for (uint8_t y = 0; y < 16; ++y) {
            for (uint8_t z = 0; z < 16; ++z) {
                for (uint8_t x = 0; x < 16; ++x) {
                    sc_coords.x = x;
                    sc_coords.y = y;
                    sc_coords.z = z;
                    palette_id* bp_uid = CUB_SUBCHUNK_BP_UID(sc, sc_coords);
                    if (!*bp_uid) // Air => no rendering required
                        continue;
                    c_coords = subchunk_coords_to_chunk(sc, sc_coords);
                    blockState* bs = &CUB_GET_BP_ELT(sc, *bp_uid)->blockstate;
                    cub_unused blockData* block = block_list->blocks + bs->BD_uid;
                    if (!block->flags.is_cubic)
                        block_render_custom(renderer, camera, block, bs,
                            chunk_coords_to_world(chunk, c_coords));
                }
            }
        }
    }
}*/

void chunk_fill(Chunk* chunk, BlockState* BS) {
    for (uint8_t i = 0; i < MAX_SUBCHUNKS; ++i) {
        SubChunk* SC = chunk->subchunks + i;
        subchunk_initialise(SC, i);
        for (uint16_t j = 0; j < 4096; ++j) {
            palette_id bp_uid = subchunk_palette_add(SC, BS);
            SC->blocks[j] = bp_uid;
        }
    }
    chunk->flags.is_modified = true;
    chunk->nb_subchunks = MAX_SUBCHUNKS;
}
