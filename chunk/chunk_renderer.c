#include "chunk/chunk_renderer.h"

// ----------------------------------------------------------------------------
void chunk_print_grid(ChunkRenderer* renderer);

void shift_grid_left(ChunkRenderer* renderer);

void shift_grid_right(ChunkRenderer* renderer);

void shift_grid_up(ChunkRenderer* renderer);

void shift_grid_down(ChunkRenderer* renderer);

// Called when the player moves towards the positive X axis by one chunk
void chunk_renderer_move_right(ChunkRenderer* renderer, BlockList* block_list);

// Called when the player moves towards the negative X axis by one chunk
void chunk_renderer_move_left(ChunkRenderer* renderer, BlockList* block_list);

// Called when the player moves towards the negative Z axis by one chunk
void chunk_renderer_move_front(ChunkRenderer* renderer, BlockList* block_list);

// Called when the player moves towards the positive Z axis by one chunk
void chunk_renderer_move_back(ChunkRenderer* renderer, BlockList* block_list);

/* Determines if the angle between the 'directional_vector' and the
 * face normal of 'face' is less than 90 degrees */
bool is_front(vec3* directional_vector, FaceID face);

/* Performs a flood-fill algorithm starting at 'org_pos' which renders the
 * visible blocks. */
void subchunk_render_dfs(BlockRenderer* block_renderer, SubChunk* SC,
                         GraphMarker* block_marker, SubChunkCoords org_pos);
// ----------------------------------------------------------------------------

const FaceID OPPOSITE_FACE[] = { FID_BOTTOM, FID_TOP,
                                 FID_BACK, FID_FRONT,
                                 FID_RIGHT, FID_LEFT };

const vec3 FACE_NORMALS[] = { VEC3_CONST(0.0f, 1.0f, 0.0f),     // TOP
                              VEC3_CONST(0.0f, -1.0f, 0.0f),    // BOTTOM
                              VEC3_CONST(0.0f, 0.0f, 1.0f),    // FRONT
                              VEC3_CONST(0.0f, 0.0f, -1.0f),     // BACK
                              VEC3_CONST(-1.0f, 0.0f, 0.0f),     // LEFT
                              VEC3_CONST(1.0f, 0.0f, 0.0f) };  // RIGHT

const struct cubGridPosition FACE_NORMALS_INT[] = {
    { .x = 0, .y = 1, .z = 0 },     // TOP
    { .x = 0, .y = -1, .z = 0 },    // BOTTOM
    { .x = 0, .y = 0, .z = 1},      // FRONT
    { .x = 0, .y = 0, .z = -1 },    // BACK
    { .x = -1, .y = 0, .z = 0},      // LEFT
    { .x = 1, .y = 0, .z = 0 }     // RIGHT
};

void chunk_print_grid(ChunkRenderer* renderer) {
    printf("------------------ Chunk GRID ----------------------\n");
    for (uint8_t i = 0; i < renderer->grid_side; ++i) {
        for (uint8_t j = 0; j < renderer->grid_side; ++j) {
            Chunk* loaded = renderer->chunk_grid + i * renderer->grid_side + j;
            printf("(%+i, %+i)   ", loaded->chunkX, loaded->chunkZ);
        }
        printf("\n");
    }
    printf("----------------------------------------------------\n");
}

void shift_grid_up(ChunkRenderer* renderer) {
    for (size_t i = renderer->grid_side; i < renderer->nb_chunks; ++i)
        renderer->chunk_grid[i - renderer->grid_side] = renderer->chunk_grid[i];
}

void shift_grid_down(ChunkRenderer* renderer) {
    for (size_t i = renderer->nb_chunks - 1; i >= renderer->grid_side; --i)
        renderer->chunk_grid[i] = renderer->chunk_grid[i - renderer->grid_side];
}

void shift_grid_left(ChunkRenderer* renderer) {
    for (size_t z = 0; z < renderer->grid_side; ++z) {
        for (size_t x = 0; x < (size_t)(renderer->grid_side - 1); ++x) {
            size_t index = z * renderer->grid_side + x;
            renderer->chunk_grid[index] = renderer->chunk_grid[index + 1];
        }
    }
}

void shift_grid_right(ChunkRenderer* renderer) {
    for (size_t z = 0; z < renderer->grid_side; ++z) {
        for (size_t x = renderer->grid_side - 1; x > 0; --x) {
            size_t index = z * renderer->grid_side + x;
            renderer->chunk_grid[index] = renderer->chunk_grid[index - 1];
        }
    }
}

void chunk_renderer_init_grid(ChunkRenderer* renderer, BlockList* block_list) {
    for (size_t z = 0; z < renderer->grid_side; ++z) {
        for (size_t x = 0; x < renderer->grid_side; ++x) {
            size_t index = z * renderer->grid_side + x;
            renderer->chunk_grid[index].chunkX = renderer->last_chunkX + x -
                                                 renderer->render_distance + 1;
            renderer->chunk_grid[index].chunkZ = renderer->last_chunkZ + z -
                                                 renderer->render_distance + 1;
            chunk_load(renderer->chunk_grid + index, block_list,
                       renderer->block_marker);
        }
    }
    chunk_print_grid(renderer);
}

void chunk_renderer_unload_grid(ChunkRenderer* renderer) {
    for (size_t z = 0; z < renderer->grid_side; ++z)
        for (size_t x = 0; x < renderer->grid_side; ++x)
            chunk_unload(renderer->chunk_grid + z * renderer->grid_side + x);
}

void chunk_renderer_setup(ChunkRenderer* renderer, BlockList* block_list,
                          vec3 position, uint8_t render_distance) {
    renderer->grid_side = render_distance * 2 - 1;
    renderer->render_distance = render_distance;
    renderer->block_marker = utils_graph_marker_new(4096);
    renderer->SC_queue = queue_new();
    renderer->RSC_queue = queue_new();
    renderer->nb_chunks = renderer->grid_side * renderer->grid_side;
    renderer->chunk_grid = utils_malloc(renderer->nb_chunks * sizeof(Chunk),
                                        "chunk_renderer_setup");
    renderer->last_chunkX = (int32_t)(floorf(position.coords[0] / 16.0f));
    renderer->last_chunkZ = (int32_t)(floorf(position.coords[2] / 16.0f));
    chunk_renderer_init_grid(renderer, block_list);
}


void chunk_renderer_move_right(ChunkRenderer* renderer, BlockList* block_list) {
    for (size_t i = 0; i < renderer->grid_side; ++i)
        chunk_unload(renderer->chunk_grid + i * renderer->grid_side);
    shift_grid_left(renderer);
    for (size_t i = 0; i < renderer->grid_side; ++i) {
        size_t index = renderer->grid_side * (i + 1) - 1;
        renderer->chunk_grid[index].chunkX = renderer->last_chunkX +
                                             renderer->render_distance - 1;
        renderer->chunk_grid[index].chunkZ = renderer->last_chunkZ + i -
                                             renderer->render_distance + 1;
        chunk_load(renderer->chunk_grid + index, block_list,
                   renderer->block_marker);
    }
}

void chunk_renderer_move_left(ChunkRenderer* renderer, BlockList* block_list) {
    for (size_t i = 0; i < renderer->grid_side; ++i)
        chunk_unload(renderer->chunk_grid + renderer->grid_side * (i + 1) - 1);
    shift_grid_right(renderer);
    for (size_t i = 0; i < renderer->grid_side; ++i) {
        size_t index = renderer->grid_side * i;
        renderer->chunk_grid[index].chunkX = renderer->last_chunkX -
                                             renderer->render_distance + 1;
        renderer->chunk_grid[index].chunkZ = renderer->last_chunkZ + i -
                                             renderer->render_distance + 1;
        chunk_load(renderer->chunk_grid + index, block_list,
                   renderer->block_marker);
    }
}

void chunk_renderer_move_back(ChunkRenderer* renderer, BlockList* block_list) {
    for (size_t i = 0; i < renderer->grid_side; ++i)
        chunk_unload(renderer->chunk_grid + renderer->nb_chunks - i - 1);
    shift_grid_down(renderer);
    for (size_t i = 0; i < renderer->grid_side; ++i) {
        renderer->chunk_grid[i].chunkX = renderer->last_chunkX + i -
                                         renderer->render_distance + 1;
        renderer->chunk_grid[i].chunkZ = renderer->last_chunkZ -
                                         renderer->render_distance + 1;
        chunk_load(renderer->chunk_grid + i, block_list,
                   renderer->block_marker);
    }
}

void chunk_renderer_move_front(ChunkRenderer* renderer, BlockList* block_list) {
    for (size_t i = 0; i < renderer->grid_side; ++i)
        chunk_unload(renderer->chunk_grid + i);
    shift_grid_up(renderer);
    for (size_t i = 0; i < renderer->grid_side; ++i) {
        size_t index = renderer->nb_chunks - renderer->grid_side + i;
        renderer->chunk_grid[index].chunkX = renderer->last_chunkX + i -
                                             renderer->render_distance + 1;
        renderer->chunk_grid[index].chunkZ = renderer->last_chunkZ +
                                             renderer->render_distance - 1;
        chunk_load(renderer->chunk_grid + index, block_list,
                   renderer->block_marker);
    }
}

void chunk_renderer_update(ChunkRenderer* renderer, BlockList* block_list,
                           vec3 position) {
    int32_t prev_chunkX = renderer->last_chunkX;
    int32_t prev_chunkZ = renderer->last_chunkZ;
    renderer->last_chunkX = (int32_t)(floorf(position.coords[0] / 16.0f));
    renderer->last_chunkZ = (int32_t)(floorf(position.coords[2] / 16.0f));
    if (renderer->last_chunkX == prev_chunkX + 1)
        chunk_renderer_move_right(renderer, block_list);
    else if (renderer->last_chunkX == prev_chunkX - 1)
        chunk_renderer_move_left(renderer, block_list);
    if (renderer->last_chunkZ == prev_chunkZ + 1)
        chunk_renderer_move_front(renderer, block_list);
    else if (renderer->last_chunkZ == prev_chunkZ - 1)
        chunk_renderer_move_back(renderer, block_list);
    if (renderer->last_chunkX != prev_chunkX ||
        renderer->last_chunkZ != prev_chunkZ)
        chunk_print_grid(renderer);
}

bool is_front(vec3* directional_vector, FaceID face) {
    return VEC3_DOT(directional_vector, (vec3*)FACE_NORMALS + face) >= 0.0f;
}

void chunk_renderer_render(ChunkRenderer* renderer,
                           BlockRenderer* block_renderer,
                           Camera* camera) {
    const float sphere_radius = 16.0f;
    Queue* Q = renderer->SC_queue;
    Queue* R = renderer->RSC_queue;
    queue_clear(Q);
    queue_clear(R);

    size_t grid_center_index = (renderer->render_distance - 1) *
                               (renderer->grid_side + 1);
    uint8_t SC_y_pos = (uint8_t)(camera->position.coords[1] / 16.0f);

    SubChunk* current_SC = renderer->chunk_grid[grid_center_index].subchunks
                         + SC_y_pos;
    current_SC->visibility_graph[FID_ENTRANCES] = 0xFF;
    current_SC->BFS_data.position.x = renderer->render_distance - 1;
    current_SC->BFS_data.position.y = SC_y_pos;
    current_SC->BFS_data.position.z = renderer->render_distance - 1;
    queue_enqueue(R, current_SC);

    struct cubGridPosition current_position = {
        .x = renderer->render_distance - 1,
        .y = SC_y_pos,
        .z = renderer->render_distance - 1
    };
    struct cubGridPosition next_position;
    Chunk* next_chunk;
    SubChunk* next_SC;
    vec3 SC_sphere_center;

    for (FaceID face = FID_START; face <= FID_END; ++face) {
        if (current_SC->visibility_graph[face]) {
            next_position.x = current_position.x + FACE_NORMALS_INT[face].x;
            if (next_position.x < 0 || next_position.x >= renderer->grid_side)
                continue; // Unreachable
            next_position.y = current_position.y + FACE_NORMALS_INT[face].y;
            if (next_position.y < 0 || next_position.y >= MAX_SUBCHUNKS)
                continue; // Unreachable
            next_position.z = current_position.z + FACE_NORMALS_INT[face].z;
            if (next_position.z < 0 || next_position.z >= renderer->grid_side)
                continue; // Unreachable
            next_chunk = renderer->chunk_grid +
                         next_position.z * renderer->grid_side +
                         next_position.x;
            SC_sphere_center = VEC3(next_chunk->chunkX * 16.0f + 8.0f,
                                    next_position.y * 16.0f + 8.0f,
                                    next_chunk->chunkZ * 16.0f + 8.0f);
            if (is_sphere_in_frustum(&camera->frustum, SC_sphere_center,
                                     sphere_radius)) {
                next_SC = next_chunk->subchunks + next_position.y;
                next_SC->BFS_data.position = next_position;
                next_SC->BFS_data.src_face = OPPOSITE_FACE[face];
                next_SC->BFS_data.directional_vector = FACE_NORMALS[face];
                next_SC->visibility_graph[FID_ENTRANCES] =
                    FACE_BITMASK(OPPOSITE_FACE[face]);
                queue_enqueue(Q, next_SC);
            }
        }
    }

    while (!queue_is_empty(Q)) {
        current_SC = queue_dequeue(Q);
        current_position = current_SC->BFS_data.position;
        queue_enqueue(R, current_SC);
        for (FaceID face = FID_START; face <= FID_END; ++face) {
            if (current_SC->visibility_graph[current_SC->BFS_data.src_face]
                & FACE_BITMASK(face)
                && is_front(&current_SC->BFS_data.directional_vector, face)) {
                next_position.x = current_position.x + FACE_NORMALS_INT[face].x;
                if (next_position.x < 0 ||
                    next_position.x >= renderer->grid_side)
                    continue; // Unreachable
                next_position.y = current_position.y + FACE_NORMALS_INT[face].y;
                if (next_position.y < 0 ||
                    next_position.y >= MAX_SUBCHUNKS)
                    continue; // Unreachable
                next_position.z = current_position.z + FACE_NORMALS_INT[face].z;
                if (next_position.z < 0 ||
                    next_position.z >= renderer->grid_side)
                    continue; // Unreachable
                next_chunk = renderer->chunk_grid +
                             next_position.z * renderer->grid_side +
                             next_position.x;
                next_SC = next_chunk->subchunks + next_position.y;
                if (next_SC->visibility_graph[FID_ENTRANCES]) {
                    next_SC->visibility_graph[FID_ENTRANCES] |=
                        FACE_BITMASK(OPPOSITE_FACE[face]);
                    continue; // Already marked
                }
                SC_sphere_center = VEC3(next_chunk->chunkX * 16.0f + 8.0f,
                                        next_position.y * 16.0f + 8.0f,
                                        next_chunk->chunkZ * 16.0f + 8.0f);
                if (is_sphere_in_frustum(&camera->frustum, SC_sphere_center,
                                         sphere_radius)) {
                    next_SC->BFS_data.position = next_position;
                    next_SC->BFS_data.src_face = OPPOSITE_FACE[face];
                    next_SC->BFS_data.directional_vector =
                        VEC3_ADD(current_SC->BFS_data.directional_vector,
                                 FACE_NORMALS[face]);
                    next_SC->visibility_graph[FID_ENTRANCES] =
                        FACE_BITMASK(OPPOSITE_FACE[face]);
                    queue_enqueue(Q, next_SC);
                }
            }
        }
    }

    Chunk* current_chunk;
    size_t nb_sc_rendered = 0;
    while (!queue_is_empty(R)) {
        current_SC = queue_dequeue(R);
        current_chunk = renderer->chunk_grid +
                        current_SC->BFS_data.position.z * renderer->grid_side +
                        current_SC->BFS_data.position.x;
        if (current_SC->y_pos < MAX_SUBCHUNKS) {
            subchunk_render(renderer, block_renderer,
                            current_chunk, current_SC);
            ++nb_sc_rendered;
        }
        current_SC->visibility_graph[FID_ENTRANCES] = 0;
    }
    printf("NB_subchunk_rendered = %lu\n", nb_sc_rendered);
}

#define FLOOD_FILL_NEIGHBOR_CHECK(COORDINATE, upper_bound,\
                                  faceID, opposite_faceID) \
    if (org_pos.COORDINATE > 0) {\
        org_pos.COORDINATE -= 1;\
    neighbor = subchunk_get_BP_elt(SC, org_pos);\
        neighbor_id = CUB_SUBCHUNK_BLOCK_ID(SC, org_pos);\
        if (!utils_graph_marker_is_marked(block_marker, neighbor_id)) {\
            if (!neighbor) {\
                subchunk_render_dfs(block_renderer, SC, block_marker, org_pos);\
            } else if(!neighbor->BS.block->flags.is_opaque) {\
                if (!neighbor->BS.block->flags.is_cubic)\
                    block_custom_render(&block_renderer->custom,\
                                        &neighbor->BS, org_pos);\
                subchunk_render_dfs(block_renderer, SC, block_marker, org_pos);\
            } else if (neighbor->BS.block->flags.is_cubic) {\
                if (block_cubic_add_to_cache(&block_renderer->cubic,\
                                             &neighbor->BS, opposite_faceID,\
                                             org_pos))\
                    glUseProgram(block_renderer->custom.shader);\
            } else {\
                block_custom_render(&block_renderer->custom,\
                                    &neighbor->BS, org_pos);\
            }\
        }\
        org_pos.COORDINATE += 1;\
    }\
    if (org_pos.COORDINATE < upper_bound - 1) {\
        org_pos.COORDINATE += 1;\
        neighbor = subchunk_get_BP_elt(SC, org_pos);\
        neighbor_id = CUB_SUBCHUNK_BLOCK_ID(SC, org_pos);\
        if (!utils_graph_marker_is_marked(block_marker, neighbor_id)) {\
            if (!neighbor) {\
                subchunk_render_dfs(block_renderer, SC, block_marker, org_pos);\
            } else if (!neighbor->BS.block->flags.is_opaque) {\
                if (!neighbor->BS.block->flags.is_cubic)\
                    block_custom_render(&block_renderer->custom,\
                                        &neighbor->BS, org_pos);\
                subchunk_render_dfs(block_renderer, SC, block_marker, org_pos);\
            } else if (neighbor->BS.block->flags.is_cubic) {\
                if (block_cubic_add_to_cache(&block_renderer->cubic,\
                                             &neighbor->BS, faceID, org_pos))\
                    glUseProgram(block_renderer->custom.shader);\
            } else {\
                block_custom_render(&block_renderer->custom,\
                                    &neighbor->BS, org_pos);\
            }\
        }\
        org_pos.COORDINATE -= 1;\
    }

void subchunk_render_dfs(BlockRenderer* block_renderer, SubChunk* SC,
                         GraphMarker* block_marker, SubChunkCoords org_pos) {
    utils_graph_marker_mark(block_marker, CUB_SUBCHUNK_BLOCK_ID(SC, org_pos));
    uint16_t neighbor_id;
    BP_elt* neighbor;
    FLOOD_FILL_NEIGHBOR_CHECK(x, CHUNK_WIDTH, FID_LEFT, FID_RIGHT);
    FLOOD_FILL_NEIGHBOR_CHECK(y, SUBCHUNK_HEIGHT, FID_BOTTOM, FID_TOP);
    FLOOD_FILL_NEIGHBOR_CHECK(z, CHUNK_DEPTH, FID_BACK, FID_FRONT);
}

#define FLOOD_FILL_FACE_ROUTINE(FACE, COORD_I, COORD_J,\
                                COORD_CONST, CONST_VALUE)\
    if (SC->visibility_graph[FID_ENTRANCES] & FACE_BITMASK(FACE)) {\
        for (uint8_t i = 0; i < CHUNK_WIDTH; ++i) {\
            for (uint8_t j = 0; j < CHUNK_DEPTH; ++j) {\
                org_pos.COORD_I = i; org_pos.COORD_J = j;\
                org_pos.COORD_CONST = CONST_VALUE;\
                org = subchunk_get_BP_elt(SC, org_pos);\
                org_id = CUB_SUBCHUNK_BLOCK_ID(SC, org_pos);\
                if (org) {\
                    if (org->BS.block->flags.is_cubic) {\
                        if (block_cubic_add_to_cache(&block_renderer->cubic,\
                                                     &org->BS, FACE, org_pos))\
                            glUseProgram(block_renderer->custom.shader);\
                    } else\
                        block_custom_render(&block_renderer->custom,\
                                            &org->BS, org_pos);\
                }\
                if (!utils_graph_marker_is_marked(\
                    renderer->block_marker, org_id)\
                    && (!org || !org->BS.block->flags.is_opaque)) {\
                    subchunk_render_dfs(block_renderer, SC,\
                                        renderer->block_marker, org_pos);\
                }\
            }\
        }\
    }

void subchunk_render(ChunkRenderer* renderer, BlockRenderer* block_renderer,
                     Chunk* C, SubChunk* SC) {
    utils_graph_marker_init(renderer->block_marker);
    SubChunkCoords org_pos;
    palette_id org_id;
    BP_elt* org;
    glUseProgram(block_renderer->cubic.shader);
    glUniform3f(block_renderer->cubic.uniforms.subchunk_pos,
                (GLfloat)C->chunkX, (GLfloat)SC->y_pos, (GLfloat)C->chunkZ);
    glUseProgram(block_renderer->custom.shader);
    glUniform3f(block_renderer->custom.uniforms.subchunk_pos,
                (GLfloat)C->chunkX, (GLfloat)SC->y_pos, (GLfloat)C->chunkZ);
    FLOOD_FILL_FACE_ROUTINE(FID_TOP, x, z, y, SUBCHUNK_HEIGHT - 1);
    FLOOD_FILL_FACE_ROUTINE(FID_BOTTOM, x, z, y, 0);
    FLOOD_FILL_FACE_ROUTINE(FID_FRONT, x, y, z, CHUNK_DEPTH - 1);
    FLOOD_FILL_FACE_ROUTINE(FID_BACK, x, y, z, 0);
    FLOOD_FILL_FACE_ROUTINE(FID_LEFT, z, y, x, 0);
    FLOOD_FILL_FACE_ROUTINE(FID_RIGHT, z, y, x, CHUNK_WIDTH - 1);
    block_cubic_flush_cache(&block_renderer->cubic);
    glUseProgram(block_renderer->custom.shader);
}

void chunk_renderer_free(ChunkRenderer* renderer) {
    chunk_renderer_unload_grid(renderer);
    utils_graph_marker_free(renderer->block_marker);
    queue_free(renderer->SC_queue);
    queue_free(renderer->RSC_queue);
    free(renderer->chunk_grid);
}
