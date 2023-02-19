#ifndef CUB_CHUNK_CHUNK_H
#define CUB_CHUNK_CHUNK_H

#include "block_renderer.h"
#include "block_utils.h"
#include "chunk_utils.h"

/* Set the block_id of the block at position (x, y, z) in CHUNK coordinates */
void chunk_set_block(GraphMarker* graph_marker, Chunk* chunk,
                     ChunkCoords coords, BlockState* BS);

/* Fills a chunk with the provided blockstate */
void chunk_fill(Chunk* chunk, BlockState* BS);

#endif
