#ifndef CUB_RENDER_CHUNK_LOADER_H
#define CUB_RENDER_CHUNK_LOADER_H

#include "render/block_utils.h"
#include "render/chunk_utils.h"

palette_id chunk_blockstate_uid(blockRenderer* renderer,
                                        blockState* block);

void chunk_load_blocks(subChunk* sc, palette_id* new_ids,
                        uint16_t palette_len, FILE* fp);

void chunk_load_palette(subChunk* sc, blockRenderer* renderer,
                         FILE* fp);

/* Load the chunk and allocate its memory */
void chunk_load(chunk* chunk, blockRenderer* renderer);

void chunk_save_blocks(hashMap* new_ids, uint16_t palette_len,
                        palette_id* blocks, FILE* fp);

void chunk_save_palette(subChunk* sc, blockRenderer* renderer,
                         FILE* fp);

/* Save the chunk and FREE all its allocated memory */
void chunk_save(chunk* chunk, blockRenderer* renderer);

#endif
