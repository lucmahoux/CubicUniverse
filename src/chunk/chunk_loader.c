#include "chunk_loader.h"

// ----------------------------------------------------------------------------
static void chunk_load_blocks(SubChunk* SC, palette_id palette_len,
                       BP_elt** BP_elts, FILE* fp);

static void chunk_load_palette(SubChunk* SC, BlockList* block_list, FILE* fp);

static void chunk_save_blocks(const palette_id* new_ids_mapper, palette_id palette_len,
                       const palette_id* blocks, FILE* fp);

static void chunk_save_palette(SubChunk* SC, FILE* fp);
//-----------------------------------------------------------------------------

static void chunk_load_blocks(SubChunk* SC, palette_id palette_len,
                       BP_elt** BP_elts, FILE* fp) {
    const char fname[] = "chunk_load_blocks";
    SC->blocks = utils_malloc(4096 * sizeof(palette_id), fname);
    if (palette_len <= 255) {
        uint8_t ids_buffer[1024];
        for (uint8_t i = 0; i < 4; ++i) {
            utils_fread(ids_buffer, sizeof(uint8_t), 1024,
                        fp, fname, "batch_of_blocks");
            for (uint16_t j = 0; j < 1024; ++j) {
                SC->blocks[(i << 10) + j] = ids_buffer[j];
                if (ids_buffer[j] != AIR_ID)
                    ++BP_elts[ids_buffer[j] - 1]->nb_blocks;
            }
        }
    } else {
        uint16_t ids_buffer[512];
        for (uint8_t i = 0; i < 8; ++i) {
            utils_fread(ids_buffer, sizeof(uint16_t), 512,
                        fp, fname, "batch_of_blocks");
            for (uint16_t j = 0; j < 512; ++j) {
                SC->blocks[(i << 9) + j] = ids_buffer[j];
                if (ids_buffer[j] != AIR_ID)
                    ++BP_elts[ids_buffer[j] - 1]->nb_blocks;
            }
        }
    }
}

static void chunk_load_palette(SubChunk* SC, BlockList* block_list, FILE* fp) {
    const char fname[] = "chunk_load_palette";
    palette_id palette_len;
    utils_fread(&palette_len, sizeof(palette_id), 1, fp, fname, "palette_len");
    if (palette_len > PALETTE_DEFAULT_LEN)
        SC->palette.BS_uid_mapper = hashmap_new(palette_len, 0);
    else
        SC->palette.BS_uid_mapper = hashmap_new(PALETTE_DEFAULT_LEN,
                                                PALETTE_DEFAULT_HASH_NBR);
    SC->palette.BP_elts = list_new(sizeof(BP_elt*), REALLOC_INCREMENT,
                                   palette_len);
    for (palette_id i = 0; i < palette_len; ++i) {
        BP_elt* elt = utils_malloc(sizeof(BP_elt), fname);
        list_append(SC->palette.BP_elts, &elt);
        // Palette Elements Loading
        BDuid BD_uid;
        utils_fread(&BD_uid, sizeof(BDuid), 1, fp, fname, "palette_block_uid");
        elt->BS.block = block_list->blocks + BD_uid;
        elt->nb_blocks = 0;
        uint8_t nb_states = elt->BS.block->nb_states;
        elt->BS.states = utils_malloc(nb_states * sizeof(bs_val), fname);
        utils_fread(elt->BS.states, sizeof(bs_val), nb_states,
                    fp, fname, "palette_states");
        // Set the uid of the blockstate
        block_set_BS_uid(&elt->BS);
        hashmap_set(SC->palette.BS_uid_mapper, elt->BS.BS_uid,
                    (void*)(size_t)(i + 1));
    }
    // Load the batch of blocks
    chunk_load_blocks(SC, palette_len, (BP_elt**)SC->palette.BP_elts->data, fp);
}

void chunk_load(Chunk* chunk, BlockList* block_list,
                GraphMarker* block_marker) {
    char path[100];
    sprintf(path, "%schunks/%i.%i.chunk", ASSETS_PATH,
                    chunk->chunkX, chunk->chunkZ);
    printf("chunk_load: Loading chunk at %s!\n", path);
    FILE* fp;
    if ( (fp = fopen(path, "rb")) == NULL ) {
        printf("chunk_load: File not found: %s - assuming not generated\n",
                path);
        chunk_init(chunk, chunk->chunkX, chunk->chunkZ);
    } else {
        const char fname[] = "chunk_load";
        utils_fread(&chunk->nb_subchunks, sizeof(uint8_t), 1,
                    fp, fname, "nb_subchunks");
        uint8_t nb_subchunks = chunk->nb_subchunks;
        // Initialise the subchunks array
        for (uint8_t i = 0; i < MAX_SUBCHUNKS; ++i) {
            SubChunk* subchunk = chunk->subchunks + i;
            subchunk->y_pos = MAX_SUBCHUNKS;
            memset(subchunk->visibility_graph, 0x3f, 6 * sizeof(uint8_t));
            subchunk->visibility_graph[FID_ENTRANCES] = 0;
        }
        while (nb_subchunks--) {
            // Load subChunk
            uint8_t y_pos;
            size_t nb_read = fread(&y_pos, sizeof(uint8_t), 1, fp);
            if (nb_read != 1 || y_pos >= MAX_SUBCHUNKS)
                errx(1, "chunk_load: Wrong format 'Y_pos subchunk'!");
            SubChunk* subchunk = &chunk->subchunks[y_pos];
            subchunk->y_pos = y_pos;
            // Palette loading & blocks loading
            chunk_load_palette(subchunk, block_list, fp);
            subchunk_build_visibility_graph(subchunk, block_marker);
        }
        chunk->flags.is_modified = false;
        chunk->flags.is_loaded = true;
        fclose(fp);
        printf("chunk_load: Chunk at %s is loaded!\n", path);
    }
}

static void chunk_save_blocks(const palette_id* new_ids_mapper, palette_id palette_len,
                       const palette_id* blocks, FILE* fp) {
    const char fname[] = "chunk_save_blocks";
    if (palette_len <= 255) {
        uint8_t buffer[1024];
        for (uint8_t i = 0; i < 4; ++i) {
            for (uint16_t j = 0; j < 1024; ++j)
                buffer[j] = new_ids_mapper[blocks[(i << 10) + j]];
            utils_fwrite(buffer, sizeof(uint8_t), 1024,
                         fp, fname, "batch_of_blocks");
        }
    } else {
        uint16_t buffer[512];
        for (uint8_t i = 0; i < 8; ++i) {
            for (uint16_t j = 0; j < 512; ++j)
                buffer[i] = new_ids_mapper[blocks[(i << 9) + j]];
            utils_fwrite(buffer, sizeof(uint16_t), 512,
                         fp, fname, "batch_of_blocks");
        }
    }
}

static void chunk_save_palette(SubChunk* SC, FILE* fp) {
    const char fname[] = "chunk_save_palette";
    palette_id palette_len = SC->palette.BS_uid_mapper->nb_keys;
    utils_fwrite(&palette_len, sizeof(palette_id), 1, fp, fname, "palette_len");
    palette_id* new_ids_mapper =
        utils_malloc((SC->palette.BP_elts->len + 1) * sizeof(palette_id),
                     fname);
    BP_elt** BP_elts = SC->palette.BP_elts->data;
    palette_id last_id = 1;
    new_ids_mapper[0] = AIR_ID;
    for (size_t i = 0; i < SC->palette.BP_elts->len; ++i) {
        if (BP_elts[i] == NULL) {
            new_ids_mapper[i + 1] = AIR_ID;
            continue; // AIR
        }
        new_ids_mapper[i + 1] = last_id++;
        // Save palette element
        utils_fwrite(&BP_elts[i]->BS.block->id, sizeof(BDuid), 1,
                     fp, fname, "palette_block_id");
        uint8_t nb_states = BP_elts[i]->BS.block->nb_states;
        utils_fwrite(BP_elts[i]->BS.states, sizeof(bs_val), nb_states,
                     fp, fname, "palette_states");
        // Free palette element
        block_BP_elt_free(BP_elts[i]);
    }
    // Free palette
    hashmap_free(SC->palette.BS_uid_mapper);
    list_free(SC->palette.BP_elts);
    // Save & free blocks
    chunk_save_blocks(new_ids_mapper, palette_len, SC->blocks, fp);
    free(new_ids_mapper);
    free(SC->blocks);
}

void chunk_save(Chunk* chunk) {
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
            SubChunk* SC = chunk->subchunks + i;
            utils_fwrite(&SC->y_pos, sizeof(uint8_t), 1,
                         fp, fname, "Y_pos subchunk");
            chunk_save_palette(&chunk->subchunks[i], fp);
        }
        fclose(fp);
    }
}

void chunk_unload(Chunk* chunk) {
    if (chunk->flags.is_modified)
        chunk_save(chunk);
    else
        chunk_free(chunk);
}
