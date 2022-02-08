#ifndef CUB_RENDER_CHUNK_H
#define CUB_RENDER_CHUNK_H

#include "render/block.h"
#include "utils/utils.h"
#include "utils/list.h"

#define CUB_CHUNK_PALETTE(SC_ptr) ((cubPaletteTuple*)SC_ptr->palette->data)
#define CUB_MAX_SUBCHUNKS 16

typedef struct cubPaletteTuple {
    cub_block_t block_id; // Block ID for palette element
    uint16_t nb_blocks; // Number of blocks with 'block_id' as ID in subchunk
} cubPaletteTuple;

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

/* Called to initialise *subchunk and its fields (malloc, ...) */
void cub_subchunk_initialise(cubSubChunk* subchunk, uint8_t y_pos);

/* Tries to add 'block_id' to the palette of subchunk.
 * Returns the block_palette_id of the palette element related to 'block_id'
 * if found (or added), and returns 0 (= air) on failure when the palette
 * list is full (Maximum Length = 255) */
uint8_t cub_subchunk_palette_add(cubSubChunk* subchunk, cub_block_t block_id);

/* Set the block_id of the block at position (x, y, z) in CHUNK coordinates */
void cub_chunk_set_block(cubChunk* chunk, uint8_t x, uint8_t y,
                         uint8_t z, cub_block_t id);

void cub_chunk_render(cubChunk* chunk, cubBlockRenderer* block_renderer);

/* Creates an empty chunk with no subchunks */
cubChunk cub_chunk_create(uint32_t chunkX, uint32_t chunkZ);

/* Used for tests, fills a chunk with the given block id */
void cub_chunk_fill(cubChunk* chunk, cub_block_t id);

#endif
