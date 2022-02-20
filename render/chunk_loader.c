#include "render/chunk_loader.h"

uint8_t get_min_bits(size_t nbr) {
    uint8_t nb_bits = 0;
    while ((nbr >>= 1))
        ++nb_bits;
    return nb_bits ? nb_bits : 1;
}

palette_id chunk_blockstate_uid(blockRenderer* renderer,
                                        blockState* block) {
    blockData* data = CUB_BLOCK_DATA(renderer, block->id);
    uint8_t len = data->block_info.nb_states;
    palette_id uid = 0;
    uint8_t nb_bits_added = 0;
    for (uint8_t i = 0; i < len; ++i) {
        uint8_t shift = get_min_bits(CUB_BSV_LEN[data->bs_keys[i]]);
        uid = (uid << shift) + block->states[i];
        nb_bits_added += shift;
    }
    uid <<= ((sizeof(palette_id) << 3) - nb_bits_added);
    return uid + block->id;
}

void chunk_load_blocks(subChunk* sc, palette_id* new_ids,
                            uint16_t palette_len, FILE* fp) {
    const char fname[] = "chunk_load_blocks";
    sc->blocks = malloc(4096 * sizeof(palette_id));
    if (!sc->blocks)
        errx(1, "chunk_load_blocks: Malloc failed!");
    if (palette_len <= 255) {
        uint8_t old_ids_buffer[1024];
        for (uint8_t i = 0; i < 4; ++i) {
            utils_fread(old_ids_buffer, sizeof(uint8_t), 1024,
                            fp, fname, "batch_of_blocks");
            palette_id dest;
            for (uint16_t j = 0; j < 1024; ++j) {
                dest = new_ids[old_ids_buffer[j]];
                if (old_ids_buffer[j])
                    ((BP_elt*)hashmap_get(sc->palette, dest))
                        ->nb_blocks++;
                sc->blocks[(i << 10) + j] = dest;
            }
        }
    } else {
        uint16_t old_ids_buffer[512];
        for (uint8_t i = 0; i < 8; ++i) {
            utils_fread(old_ids_buffer, sizeof(uint16_t), 512,
                            fp, fname, "batch_of_blocks");
            palette_id dest;
            for (uint16_t j = 0; j < 512; ++j) {
                dest = new_ids[old_ids_buffer[j]];
                if (old_ids_buffer[j])
                    ((BP_elt*)hashmap_get(sc->palette, dest))
                        ->nb_blocks++;
                sc->blocks[(i << 9) + j] = dest;
            }
        }
    }
}

void chunk_load_palette(subChunk* sc, blockRenderer* renderer,
                            FILE* fp) {
    const char fname[] = "chunk_load_palette";
    uint16_t palette_len;
    utils_fread(&palette_len, sizeof(uint16_t), 1,
                    fp, fname, "palette_len");
    if (palette_len > PALETTE_DEFAULT_LEN)
        sc->palette = hashmap_new(palette_len, 0);
    else
        sc->palette = hashmap_new(PALETTE_DEFAULT_LEN,
                                        PALETTE_DEFAULT_HASH_NBR);
    palette_id* new_ids = malloc(palette_len * sizeof(palette_id));
    if (!new_ids)
        errx(1, "chunk_load_palette: Malloc failed!");
    new_ids[0] = AIR_ID;
    for (uint16_t i = 1; i <= palette_len; ++i) {
        // Palette Elements Loading
        BP_elt* elt = malloc(sizeof(BP_elt));
        if (!elt)
            errx(1, "chunk_load_palette: Malloc failed!");
        utils_fread(&elt->block.id, sizeof(block_t), 1,
                        fp, fname, "palette_block_id");
        elt->nb_blocks = 0;
        uint8_t nb_states = CUB_BLOCK_DATA(renderer, elt->block.id)
                            ->block_info.nb_states;
        elt->block.states = malloc(nb_states * sizeof(bs_val));
        printf("palette: block_id = %u - nb_states = %u\n", elt->block.id,
                nb_states);
        if (!elt->block.states)
            errx(1, "chunk_load_palette: Malloc failed!");
        utils_fread(elt->block.states, sizeof(bs_val), nb_states,
                        fp, fname, "palette_states");
        // Get the uid of the blockstate
        new_ids[i] = chunk_blockstate_uid(renderer, &elt->block);
        hashmap_set(sc->palette, new_ids[i], elt);
    }
    // Load the batch of blocks
    chunk_load_blocks(sc, new_ids, (palette_id)palette_len, fp);
    free(new_ids);
}

void chunk_load(chunk* chunk, blockRenderer* renderer) {
    char path[100];
    sprintf(path, "%schunks/%i.%i.chunk", ASSETS_PATH,
                    chunk->chunkX, chunk->chunkZ);
    printf("chunk_load: Loading chunk at %s!\n", path);
    FILE* fp;
    if ( (fp = fopen(path, "rb")) == NULL ) {
        errx(1, "chunk_load: File not found: %s!", path);
    } else {
        const char fname[] = "chunk_load";
        utils_fread(&chunk->nb_subchunks, sizeof(uint8_t), 1,
                        fp, fname, "nb_subchunks");
        uint8_t nb_subchunks = chunk->nb_subchunks;
        // Initialise the subchunks array
        for (uint8_t i = 0; i < MAX_SUBCHUNKS; ++i)
            chunk->subchunks[i].y_pos = MAX_SUBCHUNKS;
        while (nb_subchunks--) {
            // Load subChunk
            uint8_t y_pos;
            size_t nb_read = fread(&y_pos, sizeof(uint8_t), 1, fp);
            if (nb_read != 1 || y_pos >= MAX_SUBCHUNKS)
                errx(1, "chunk_load: Wrong format 'Y_pos subchunk'!");
            subChunk* subchunk = &chunk->subchunks[y_pos];
            subchunk->y_pos = y_pos;
            // Palette loading & blocks loading
            chunk_load_palette(subchunk, renderer, fp);
        }
        fclose(fp);
    }
}

void chunk_save_blocks(hashMap* new_ids, uint16_t palette_len,
                            palette_id* blocks, FILE* fp) {
    const char fname[] = "chunk_save_blocks";
    if (palette_len <= 255) {
        for (uint16_t i = 0; i < 4096; ++i) {
            uint8_t src = (size_t)hashmap_get(new_ids, blocks[i]);
            utils_fwrite(&src, sizeof(uint8_t), 1,
                             fp, fname, "batch_of_blocks");
        }
    } else {
        for (uint16_t i = 0; i < 4096; ++i) {
            uint16_t src = (size_t)hashmap_get(new_ids, blocks[i]);
            utils_fwrite(&src, sizeof(uint16_t), 1,
                             fp, fname, "batch_of_blocks");
        }
    }
    // Free the block array
    free(blocks);
}

void chunk_save_palette(subChunk* sc, blockRenderer* renderer,
                            FILE* fp) {
    const char fname[] = "chunk_save_palette";
    uint16_t palette_len = sc->palette->nb_keys;
    utils_fwrite(&palette_len, sizeof(uint16_t), 1,
                     fp, fname, "palette_len");
    palette_id* old_ids = malloc(palette_len * sizeof(palette_id));
    if (!old_ids)
        errx(1, "chunk_save_palette: Malloc failed!");
    hashmap_get_keys_uint(sc->palette, old_ids);
    hashMap* new_ids = hashmap_new(palette_len, 0);
    for (size_t i = 1; i <= palette_len; ++i) {
        // Save palette element
        hashmap_set(new_ids, old_ids[i - 1], (void*) i);
        BP_elt* p_elt = hashmap_get(sc->palette, old_ids[i - 1]);
        utils_fwrite(&p_elt->block.id, sizeof(block_t), 1,
                         fp, fname, "palette_block_id");
        uint8_t nb_states = CUB_BLOCK_DATA(renderer, p_elt->block.id)
                            ->block_info.nb_states;
        utils_fwrite(p_elt->block.states, sizeof(bs_val), nb_states,
                         fp, fname, "palette_states");
        // Free palette element
        free(p_elt->block.states);
        free(p_elt);
    }
    hashmap_set(new_ids, 0, (void*) 0); // Air is always 0
    // Free palette
    hashmap_free(sc->palette);
    // Free the list of old ids
    free(old_ids);
    // Save & free blocks
    chunk_save_blocks(new_ids, palette_len, sc->blocks, fp);
    // Free the hashmap [old_id -> new_id]
    hashmap_free(new_ids);
}

void chunk_save(chunk* chunk, blockRenderer* renderer) {
    char path[100];
    sprintf(path, "%schunks/%i.%i.chunk", ASSETS_PATH,
                    chunk->chunkX, chunk->chunkZ);
    printf("chunk_save: Saving chunk at %s!\n", path);
    FILE* fp;
    if ( (fp = fopen(path, "wb")) == NULL ) {
        errx(1, "chunk_save: Error while trying to save the chunk!");
    } else {
        const char fname[] = "chunk_save";
        utils_fwrite(&chunk->nb_subchunks, sizeof(uint8_t), 1,
                         fp, fname, "nb_subchunks");
        for (uint8_t i = 0; i < MAX_SUBCHUNKS; ++i) {
            if (chunk->subchunks[i].y_pos >= MAX_SUBCHUNKS)
                continue;
            subChunk* sc = chunk->subchunks + i;
            utils_fwrite(&sc->y_pos, sizeof(uint8_t), 1,
                             fp, fname, "Y_pos subchunk");
            chunk_save_palette(&chunk->subchunks[i], renderer, fp);
        }
        fclose(fp);
    }
}

