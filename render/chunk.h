#ifndef CUB_RENDER_CHUNK_H
#define CUB_RENDER_CHUNK_H

#include "render/block.h"
#include "utils/utils.h"
#include "utils/list.h"

#define CUB_MAX_SUBCHUNKS 16

typedef struct cubSubChunk {
    // Y position of the subchunk relative to the bottom of the chunk it belongs
    uint8_t y_pos;
    // List of block ids which identify the type of blocks in the subchunk
    cubList* palette;
    // List of indices in 'palette' to determine the type of block
    uint8_t* blocks;
} cubSubChunk;

typedef struct cubChunk {
    // X and Z coordinates of the chunk in the chunk coords system
    uint32_t chunkX, chunkZ;
    uint8_t nb_subchunks;
    // List of the subchunks in this chunk
    cubSubChunk subchunks[CUB_MAX_SUBCHUNKS];
} cubChunk;

/* Load the chunk and allocate the memory for the lists, ... */
void cub_chunk_load(cubChunk* chunk);

/* Save the chunk and FREE all its allocated memory*/
void cub_chunk_save(cubChunk* chunk);

/* Return the block ID at position (x, y, z) in CHUNK coordinates */
cub_block_t cub_chunk_get_block_at(cubChunk* chunk, uint8_t x,
                                    uint8_t y, uint8_t z);

/* Set the block_id of the block at position (x, y, z) in CHUNK coordinates */
void cub_chunk_set_block(cubChunk* chunk, uint8_t x, uint8_t y,
                         uint8_t z, cub_block_t id);

void cub_chunk_render(cubChunk* chunk, cubBlockRenderer* block_renderer);

#endif
