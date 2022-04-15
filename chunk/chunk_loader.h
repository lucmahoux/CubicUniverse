#ifndef CUB_CHUNK_CHUNK_LOADER_H
#define CUB_CHUNK_CHUNK_LOADER_H

#include "block/block_utils.h"
#include "chunk/chunk_utils.h"

/* Load the chunk and allocate its memory */
void chunk_load(Chunk* chunk, BlockList* block_list,
                GraphMarker* block_marker);

/* Save the chunk and FREE all its allocated memory */
void chunk_save(Chunk* chunk);

/* Unload the chunk by freeing it - if chunk was modified, it saves it */
void chunk_unload(Chunk* chunk);

#endif
