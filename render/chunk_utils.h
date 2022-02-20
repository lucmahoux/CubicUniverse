#ifndef CUB_RENDER_CHUNK_UTILS_H
#define CUB_RENDER_CHUNK_UTILS_H

#include "utils/utils.h"
#include "utils/hashmap.h"

#define MAX_SUBCHUNKS 16
#define PALETTE_DEFAULT_LEN 30
#define PALETTE_DEFAULT_HASH_NBR 31
#define palette_id uint32_t

typedef struct subChunk {
    // Y position of the subchunk relative to the bottom of the chunk it belongs
    uint8_t y_pos;
    // Hashmap linking each palette_id to a BP_elt
    hashMap* palette;
    // List of indices in 'palette' to determine the type of block
    palette_id* blocks;
} subChunk;

typedef struct chunk {
    // X and Z coordinates of the chunk in the chunk coords system
    uint32_t chunkX, chunkZ;
    uint8_t nb_subchunks;
    // List of the subchunks in this chunk
    subChunk subchunks[MAX_SUBCHUNKS];
} chunk;

// ---------- Useful Macros ---------- //
#define CUB_GET_BP_ELT(subchunk, bp_uid) ((BP_elt*)\
        hashmap_get(subchunk->palette, (size_t)bp_uid))
#define CUB_SUBCHUNK_BP_UID(sc, x, y, z) (sc->blocks + x + (z << 4) + (y << 8))

#endif
