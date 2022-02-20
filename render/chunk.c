#include "render/chunk.h"

void chunk_get_block_at(chunk* chunk, uint8_t x,
                            uint8_t y, uint8_t z, blockState* block) {
    subChunk* sc = &chunk->subchunks[y >> 4];
    if (sc->y_pos >= MAX_SUBCHUNKS)
        block->id = AIR_ID; // Empty subchunk => full of air
    else {
        uint16_t p_id = sc->blocks[x + (z << 4) + (y << 8)];
        if (!p_id) block->id = AIR_ID;
        else *block = ((BP_elt*)hashmap_get(sc->palette, p_id))
                        ->block;
    }
}

void subchunk_initialise(subChunk* sc, uint8_t y_pos) {
    sc->palette = hashmap_new(PALETTE_DEFAULT_LEN,
                                    PALETTE_DEFAULT_HASH_NBR);
    sc->y_pos = y_pos;
    sc->blocks = malloc(4096 * sizeof(palette_id));
    if (!sc->blocks)
        errx(1, "subchunk_create: Malloc failed!");
    memset(sc->blocks, 0, 4096 * sizeof(palette_id));
}

palette_id subchunk_palette_add(blockRenderer* renderer,
                                        subChunk* sc, blockState* bs) {
    palette_id bp_uid = chunk_blockstate_uid(renderer, bs);
    BP_elt* bp_elt = CUB_GET_BP_ELT(sc, bp_uid);
    if (bp_elt) { // BlockState already exists in palette
        ++bp_elt->nb_blocks;
    } else { // Create a new BP_elt -> default nb_blocks is already 1
        hashmap_set(sc->palette, bp_uid,
            block_BP_elt(CUB_BLOCK_DATA(renderer, bs->id),
                bs->id, bs->states));
    }
    return bp_uid;
}

void chunk_set_block(blockRenderer* renderer, chunk* chunk,
                         uint8_t x, uint8_t y, uint8_t z,
                         blockState* bs) {
    subChunk* sc = &chunk->subchunks[y >> 4];
    if (bs->id == AIR_ID) { // Block is destroyed (= replaced by air)
        if (sc->y_pos >= MAX_SUBCHUNKS)
            return; // Already empty subchunk (full of air)
        palette_id* bp_uid = CUB_SUBCHUNK_BP_UID(sc, x, y, z);
        if (*bp_uid) // Not air => update block counter
            CUB_GET_BP_ELT(sc, *bp_uid)->nb_blocks--;
        *bp_uid = 0;
    } else if (sc->y_pos >= MAX_SUBCHUNKS) { // New subchunk
        subchunk_initialise(sc, y >> 4);
        palette_id bp_uid = subchunk_palette_add(renderer, sc, bs);
        sc->blocks[x + (z << 4) + (y << 8)] = bp_uid;
    } else { // Non empty block & already existing subchunk
        palette_id* bp_uid = &sc->blocks[x + (z << 4) + (y << 8)];
        if (*bp_uid) // Not air => update block counter
            CUB_GET_BP_ELT(sc, *bp_uid)->nb_blocks--;
        *bp_uid = subchunk_palette_add(renderer, sc, bs);
    }
}

void chunk_render(chunk* chunk, blockRenderer* renderer) {
    for (uint8_t i = 0; i < MAX_SUBCHUNKS; ++i) {
        subChunk* sc = chunk->subchunks + i;
        if (sc->y_pos >= MAX_SUBCHUNKS) continue;
        for (uint8_t y = 0; y < 16; ++y) {
            for (uint8_t z = 0; z < 16; ++z) {
                for (uint8_t x = 0; x < 16; ++x) {
                    palette_id* bp_uid = CUB_SUBCHUNK_BP_UID(sc, x, y, z);
                    if (!*bp_uid) // Air => no rendering required
                        continue;
                    block_render(renderer,
                            &CUB_GET_BP_ELT(sc, *bp_uid)->block,
                            VEC3(1.0f * (chunk->chunkX << 4) + 1.0f * x,
                                     1.0f * (sc->y_pos << 4) + 1.0f * y,
                                     1.0f * (chunk->chunkZ << 4) + 1.0f * z));
                }
            }
        }
    }
}

chunk chunk_create(uint32_t chunkX, uint32_t chunkZ) {
    chunk chunk = { .chunkX = chunkX, .chunkZ = chunkZ };
    chunk.nb_subchunks = 0;
    for (uint8_t i = 0; i < MAX_SUBCHUNKS; ++i) {
        chunk.subchunks[i].y_pos = MAX_SUBCHUNKS;
        chunk.subchunks[i].palette = NULL;
        chunk.subchunks[i].blocks = NULL;
    }
    return chunk;
}

void chunk_fill(blockRenderer* renderer,
                    chunk* chunk, blockState* bs) {
    for (uint8_t i = 0; i < MAX_SUBCHUNKS; ++i) {
        subChunk* sc = chunk->subchunks + i;
        subchunk_initialise(sc, i);
        for (uint16_t j = 0; j < 4096; ++j) {
            palette_id bp_uid = subchunk_palette_add(renderer, sc, bs);
            sc->blocks[j] = bp_uid;
        }
    }
    chunk->nb_subchunks = MAX_SUBCHUNKS;
}
