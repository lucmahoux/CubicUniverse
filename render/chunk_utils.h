#ifndef CUB_RENDER_CHUNK_UTILS_H
#define CUB_RENDER_CHUNK_UTILS_H

#include "utils/utils.h"
#include "utils/hashmap.h"

#define CUB_MAX_SUBCHUNKS 16
#define CUB_PALETTE_DEFAULT_LEN 30
#define CUB_PALETTE_DEFAULT_HASH_NBR 31
#define cub_palette_id uint32_t

typedef struct cubSubChunk {
    // Y position of the subchunk relative to the bottom of the chunk it belongs
    uint8_t y_pos;
    // Hashmap linking each palette_id to a BP_elt
    cubHashMap* palette;
    // List of indices in 'palette' to determine the type of block
    cub_palette_id* blocks;
} cubSubChunk;

typedef struct cubChunk {
    // X and Z coordinates of the chunk in the chunk coords system
    uint32_t chunkX, chunkZ;
    uint8_t nb_subchunks;
    // List of the subchunks in this chunk
    cubSubChunk subchunks[CUB_MAX_SUBCHUNKS];
} cubChunk;

// ---------- Useful Macros ---------- //
#define CUB_GET_BP_ELT(subchunk, bp_uid) ((cubBP_elt*)\
        cub_utils_hashmap_get(subchunk->palette, (size_t)bp_uid))
#define CUB_SUBCHUNK_BP_UID(sc, x, y, z) (sc->blocks + x + (z << 4) + (y << 8))

#endif
