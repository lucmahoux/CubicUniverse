#include "render/chunk.h"

void cub_chunk_get_block_at(cubChunk* chunk, uint8_t x,
                            uint8_t y, uint8_t z, cubBlockState* block) {
    cubSubChunk* sc = &chunk->subchunks[y >> 4];
    if (sc->y_pos >= CUB_MAX_SUBCHUNKS)
        block->id = CUB_AIR_ID; // Empty subchunk => full of air
    else {
        uint16_t p_id = sc->blocks[x + (z << 4) + (y << 8)];
        if (!p_id) block->id = CUB_AIR_ID;
        else *block = ((cubBP_elt*)hashmap_get(sc->palette, p_id))
                        ->block;
    }
}

void cub_subchunk_initialise(cubSubChunk* sc, uint8_t y_pos) {
    sc->palette = hashmap_new(CUB_PALETTE_DEFAULT_LEN,
                                    CUB_PALETTE_DEFAULT_HASH_NBR);
    sc->y_pos = y_pos;
    sc->blocks = malloc(4096 * sizeof(cub_palette_id));
    if (!sc->blocks)
        errx(1, "cub_subchunk_create: Malloc failed!");
    memset(sc->blocks, 0, 4096 * sizeof(cub_palette_id));
}

cub_palette_id cub_subchunk_palette_add(cubBlockRenderer* renderer,
                                        cubSubChunk* sc, cubBlockState* bs) {
    cub_palette_id bp_uid = cub_chunk_blockstate_uid(renderer, bs);
    cubBP_elt* bp_elt = CUB_GET_BP_ELT(sc, bp_uid);
    if (bp_elt) { // BlockState already exists in palette
        ++bp_elt->nb_blocks;
    } else { // Create a new BP_elt -> default nb_blocks is already 1
        hashmap_set(sc->palette, bp_uid,
            cub_block_BP_elt(CUB_BLOCK_DATA(renderer, bs->id),
                bs->id, bs->states));
    }
    return bp_uid;
}

void cub_chunk_set_block(cubBlockRenderer* renderer, cubChunk* chunk,
                         uint8_t x, uint8_t y, uint8_t z,
                         cubBlockState* bs) {
    cubSubChunk* sc = &chunk->subchunks[y >> 4];
    if (bs->id == CUB_AIR_ID) { // Block is destroyed (= replaced by air)
        if (sc->y_pos >= CUB_MAX_SUBCHUNKS)
            return; // Already empty subchunk (full of air)
        cub_palette_id* bp_uid = CUB_SUBCHUNK_BP_UID(sc, x, y, z);
        if (*bp_uid) // Not air => update block counter
            CUB_GET_BP_ELT(sc, *bp_uid)->nb_blocks--;
        *bp_uid = 0;
    } else if (sc->y_pos >= CUB_MAX_SUBCHUNKS) { // New subchunk
        cub_subchunk_initialise(sc, y >> 4);
        cub_palette_id bp_uid = cub_subchunk_palette_add(renderer, sc, bs);
        sc->blocks[x + (z << 4) + (y << 8)] = bp_uid;
    } else { // Non empty block & already existing subchunk
        cub_palette_id* bp_uid = &sc->blocks[x + (z << 4) + (y << 8)];
        if (*bp_uid) // Not air => update block counter
            CUB_GET_BP_ELT(sc, *bp_uid)->nb_blocks--;
        *bp_uid = cub_subchunk_palette_add(renderer, sc, bs);
    }
}

void cub_chunk_render(cubChunk* chunk, cubBlockRenderer* renderer) {
    for (uint8_t i = 0; i < CUB_MAX_SUBCHUNKS; ++i) {
        cubSubChunk* sc = chunk->subchunks + i;
        if (sc->y_pos >= CUB_MAX_SUBCHUNKS) continue;
        for (uint8_t y = 0; y < 16; ++y) {
            for (uint8_t z = 0; z < 16; ++z) {
                for (uint8_t x = 0; x < 16; ++x) {
                    cub_palette_id* bp_uid = CUB_SUBCHUNK_BP_UID(sc, x, y, z);
                    if (!*bp_uid) // Air => no rendering required
                        continue;
                    cub_block_render(renderer,
                            &CUB_GET_BP_ELT(sc, *bp_uid)->block,
                            VEC3(1.0f * (chunk->chunkX << 4) + 1.0f * x,
                                     1.0f * (sc->y_pos << 4) + 1.0f * y,
                                     1.0f * (chunk->chunkZ << 4) + 1.0f * z));
                }
            }
        }
    }
}

cubChunk cub_chunk_create(uint32_t chunkX, uint32_t chunkZ) {
    cubChunk chunk = { .chunkX = chunkX, .chunkZ = chunkZ };
    chunk.nb_subchunks = 0;
    for (uint8_t i = 0; i < CUB_MAX_SUBCHUNKS; ++i) {
        chunk.subchunks[i].y_pos = CUB_MAX_SUBCHUNKS;
        chunk.subchunks[i].palette = NULL;
        chunk.subchunks[i].blocks = NULL;
    }
    return chunk;
}

void cub_chunk_fill(cubBlockRenderer* renderer,
                    cubChunk* chunk, cubBlockState* bs) {
    for (uint8_t i = 0; i < CUB_MAX_SUBCHUNKS; ++i) {
        cubSubChunk* sc = chunk->subchunks + i;
        cub_subchunk_initialise(sc, i);
        for (uint16_t j = 0; j < 4096; ++j) {
            cub_palette_id bp_uid = cub_subchunk_palette_add(renderer, sc, bs);
            sc->blocks[j] = bp_uid;
        }
    }
    chunk->nb_subchunks = CUB_MAX_SUBCHUNKS;
}
