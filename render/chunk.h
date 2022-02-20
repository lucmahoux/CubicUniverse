#ifndef CUB_RENDER_CHUNK_H
#define CUB_RENDER_CHUNK_H

#include "render/block.h"
#include "render/block_utils.h"
#include "render/chunk_loader.h"
#include "render/chunk_utils.h"

/* Return the block ID at position (x, y, z) in CHUNK coordinates */
void chunk_get_block_at(chunk* chunk, uint8_t x,
                            uint8_t y, uint8_t z, blockState* block);

/* Called to initialise *subchunk and its fields (malloc, ...) */
void subchunk_initialise(subChunk* subchunk, uint8_t y_pos);

/* Tries to add 'bs' to the palette (it must not be an heap allocation).
 * If it already exists, adds 1 to the block counter, otherwise create the
 * 'bs' via a heap allocation & add it to the palette.
 * Returns the bs UID in the palette. */
palette_id subchunk_palette_add(blockRenderer* renderer,
                                        subChunk* sc, blockState* bs);

/* Set the block_id of the block at position (x, y, z) in CHUNK coordinates */
void chunk_set_block(blockRenderer* renderer,
                        chunk* chunk, uint8_t x, uint8_t y, uint8_t z,
                        blockState* id);

void chunk_render(chunk* chunk, blockRenderer* renderer);

/* Creates an empty chunk with no subchunks */
chunk chunk_create(uint32_t chunkX, uint32_t chunkZ);

/* Used for tests, fills a chunk with the given block id */
void chunk_fill(blockRenderer* renderer,
                    chunk* chunk, blockState* bs);

#endif
