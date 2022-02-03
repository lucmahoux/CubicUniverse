#ifndef CUB_STRUCT_CHUNK_H
#define CUB_STRUCT_CHUNK_H

#include "struct/block.h"

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
    // List of the subchunks in this chunk
    cubList* subchunks;
} cubChunk;

cub_block_t cub_chunk_get_block_at(uint8_t x, uint8_t y, uint8_t z);

void cub_chunk_add_block(uint8_t x, uint8_t y, uint8_t z, cub_block_t id);

void cub_chunk_rm_block(uint8_t x, uint8_t y, uint8_t z);

cub_block_t cub_subchunk_get_block_at(uint8_t x, uint8_t y, uint8_t z);

void cub_subchunk_add_block(uint8_t x, uint8_t y, uint8_t z, cub_block_t id);

void cub_subchunk_rm_block(uint8_t x, uint8_t y, uint8_t z);

cubChunk* cub_chunk_load(const char* path);

cubSubChunk* cub_subchunk_load(const char* path);

#endif
