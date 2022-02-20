#ifndef CUB_RENDER_CHUNK_LOADER_H
#define CUB_RENDER_CHUNK_LOADER_H

#include "render/block_utils.h"
#include "render/chunk_utils.h"

cub_palette_id cub_chunk_blockstate_uid(blockRenderer* renderer,
                                        blockState* block);

void cub_chunk_load_blocks(cubSubChunk* sc, cub_palette_id* new_ids,
                        uint16_t palette_len, FILE* fp);

void cub_chunk_load_palette(cubSubChunk* sc, blockRenderer* renderer,
                         FILE* fp);

/* Load the chunk and allocate its memory */
void cub_chunk_load(cubChunk* chunk, blockRenderer* renderer);

void cub_chunk_save_blocks(hashMap* new_ids, uint16_t palette_len,
                        cub_palette_id* blocks, FILE* fp);

void cub_chunk_save_palette(cubSubChunk* sc, blockRenderer* renderer,
                         FILE* fp);

/* Save the chunk and FREE all its allocated memory */
void cub_chunk_save(cubChunk* chunk, blockRenderer* renderer);

#endif
