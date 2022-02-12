#include "render/chunk.h"

void cub_chunk_load(cubChunk* chunk) {
    char path[100];
    sprintf(path, "%schunks/%i.%i.chunk", ASSETS_PATH,
                    chunk->chunkX, chunk->chunkZ);
    printf("cub_chunk_load: Loading chunk at %s!\n", path);
    FILE* fp;
    if ( (fp = fopen(path, "rb")) == NULL ) {
        errx(1, "cub_chunk_load: File not found: %s!", path);
    } else {
        size_t nb_read;
        nb_read = fread(&chunk->nb_subchunks, sizeof(uint8_t), 1, fp);
        if (nb_read != 1)
            errx(1, "cub_chunk_load: Wrong format 'nb_subchunks'!");
        uint8_t nb_subchunks = chunk->nb_subchunks;
        // Initialise the subchunks array
        for (uint8_t i = 0; i < CUB_MAX_SUBCHUNKS; ++i)
            chunk->subchunks[i].y_pos = CUB_MAX_SUBCHUNKS;
        while (nb_subchunks--) {
            // Load subChunk
            uint8_t y_pos;;
            nb_read = fread(&y_pos, sizeof(uint8_t), 1, fp);
            if (nb_read != 1 || y_pos >= CUB_MAX_SUBCHUNKS)
                errx(1, "cub_chunk_load: Wrong format 'Y_pos subchunk'!");
            cubSubChunk* subchunk = &chunk->subchunks[y_pos];
            subchunk->y_pos = y_pos;
            // Palette loading
            uint8_t palette_len;
            nb_read = fread(&palette_len, sizeof(uint8_t), 1, fp);
            if (nb_read != 1)
                errx(1, "cub_chunk_load: Wrong format 'palette_len'!");
            subchunk->palette = cub_utils_list(sizeof(cubPaletteTuple));
            for (uint16_t i = 0; i < palette_len; ++i) {
                cubPaletteTuple tuple;
                nb_read = fread(&tuple.block_id, sizeof(cub_block_t), 1, fp);
                if (nb_read != 1)
                    errx(1, "cub_chunk_load: Wrong format 'palette_id'!");
                tuple.nb_blocks = 0;
                cub_utils_list_append(subchunk->palette, &tuple);
            }
            // Block loading -> 4096 blocks per subchunk
            cubPaletteTuple* palette = subchunk->palette->data;
            subchunk->blocks = malloc(4096 * sizeof(uint8_t));
            if (!subchunk->blocks)
                errx(1, "cub_chunk_load: Malloc failed :(!");
            for (uint16_t i = 0; i < 4096; ++i) {
                uint8_t bp_id;
                nb_read = fread(&bp_id, sizeof(uint8_t), 1, fp);
                if (nb_read != 1)
                    errx(1, "cub_chunk_load: Wrong format 'block_palette_id'!");
                subchunk->blocks[i] = bp_id;
                ++palette[bp_id].nb_blocks;
            }
        }
        fclose(fp);
    }
}

void cub_chunk_save(cubChunk* chunk) {
    char path[100];
    sprintf(path, "%schunks/%i.%i.chunk", ASSETS_PATH,
                    chunk->chunkX, chunk->chunkZ);
    printf("cub_chunk_save: Saving chunk at %s!\n", path);
    FILE* fp;
    if ( (fp = fopen(path, "wb")) == NULL ) {
        errx(1, "cub_chunk_save: Error while trying to save the chunk!");
    } else {
        size_t nb_written;
        nb_written = fwrite(&chunk->nb_subchunks, sizeof(uint8_t), 1, fp);
        if (nb_written != 1)
            errx(1, "cub_chunk_save: Failed to write 'nb_subchunks'!");
        for (uint8_t i = 0; i < CUB_MAX_SUBCHUNKS; ++i) {
            if (chunk->subchunks[i].y_pos >= CUB_MAX_SUBCHUNKS)
                continue;
            // Save subChunk
            cubSubChunk* subchunk = &chunk->subchunks[i];
            nb_written = fwrite(&subchunk->y_pos, sizeof(uint8_t), 1, fp);
            if (nb_written != 1)
                errx(1, "cub_chunk_save: Wrong format 'Y_pos subchunk'!");
            // Palette saving & free
            uint8_t palette_len = subchunk->palette->len;
            nb_written = fwrite(&palette_len, sizeof(uint8_t), 1, fp);
            if (nb_written != 1)
                errx(1, "cub_chunk_save: Wrong format 'palette_len'!");
            cubPaletteTuple* palette = subchunk->palette->data;
            for (uint16_t i = 0; i < palette_len; ++i) {
                nb_written = fwrite(&palette[i].block_id,
                                    sizeof(cub_block_t), 1, fp);
                if (nb_written != 1)
                    errx(1, "cub_chunk_save: Wrong format 'palette_id'!");
            }
            cub_utils_list_free(subchunk->palette);
            // Blocks saving & free
            for (uint16_t i = 0; i < 4096; ++i) {
                nb_written = fwrite(&subchunk->blocks[i],
                                    sizeof(uint8_t), 1, fp);
                if (nb_written != 1)
                    errx(1, "cub_chunk_save: Wrong format 'block_palette_id'!");
            }
            free(subchunk->blocks);
        }
        fclose(fp);
    }
}

cub_block_t cub_chunk_get_block_at(cubChunk* chunk, uint8_t x,
                                    uint8_t y, uint8_t z) {
    cubSubChunk* subchunk = &chunk->subchunks[y >> 4];
    if (subchunk->y_pos >= CUB_MAX_SUBCHUNKS)
        return 0; // Empty subchunk => full of air
    uint8_t block_palette_id = subchunk->blocks[x + (z << 4) + (y << 8)];
    if (!block_palette_id) return 0; // Air has a default ID of 0
    return CUB_CHUNK_PALETTE(subchunk)[block_palette_id - 1].block_id;
}

void cub_subchunk_initialise(cubSubChunk* subchunk, uint8_t y_pos) {
    subchunk->palette = cub_utils_list(sizeof(cubPaletteTuple));
    subchunk->y_pos = y_pos;
    subchunk->blocks = malloc(4096 * sizeof(uint8_t));
    if (!subchunk->blocks)
        errx(1, "cub_subchunk_create: Malloc failed!");
    memset(subchunk->blocks, 0, 4096 * sizeof(uint8_t));
}

uint8_t cub_subchunk_palette_add(cubSubChunk* subchunk, cub_block_t block_id) {
    cubPaletteTuple* palette = subchunk->palette->data;
    uint8_t bp_id = 0, empty_bp_id;
    cubPaletteTuple* empty_elt = NULL;
    // Find the first empty element OR find the existing 'block_id'
    // related palette element
    for (; bp_id < subchunk->palette->len; ++bp_id) {
        if (palette[bp_id].block_id == block_id) {
            ++palette[bp_id].nb_blocks;
            return bp_id + 1;
        }
        if (palette[bp_id].nb_blocks == 0) {
            // Unused palette element
            empty_elt = &palette[bp_id];
            empty_bp_id = bp_id;
            break;
        }
    }
    // Find the 'block_id' related palette element
    for (; bp_id < subchunk->palette->len; ++bp_id) {
        if (palette[bp_id].block_id == block_id) {
            ++palette[bp_id].nb_blocks;
            return bp_id + 1;
        }
    }
    // 'block_id' not linked with a palette element
    if (!empty_elt && bp_id >= 255) { // No empty element && palette full
        printf("Cannot add any more block with a different id, "
                "palette is full!\n");
        return 0; // Air block
    } else if (bp_id >= 255) { // Use an empty palette element
        empty_elt->nb_blocks = 1;
        empty_elt->block_id = block_id;
        return empty_bp_id + 1;
    } else { // Create a new palette element
        cubPaletteTuple to_add = { .block_id = block_id, .nb_blocks = 1 };
        cub_utils_list_append(subchunk->palette, &to_add);
        return bp_id + 1;
    }
}

void cub_chunk_set_block(cubChunk* chunk, uint8_t x, uint8_t y,
                         uint8_t z, cub_block_t block_id) {
    cubSubChunk* subchunk = &chunk->subchunks[y >> 4];
    if (block_id == 0) { // Block is destroyed (= replaced by air)
        if (subchunk->y_pos >= CUB_MAX_SUBCHUNKS)
            return; // Already empty subchunk (full of air)
        uint8_t* bp_id = &subchunk->blocks[x + (z << 4) + (y << 8)];
        if (*bp_id) // Not air => update block counter
            CUB_CHUNK_PALETTE(subchunk)[*bp_id - 1].nb_blocks--;
        *bp_id = 0;
    } else if (subchunk->y_pos >= CUB_MAX_SUBCHUNKS) { // New subchunk
        cub_subchunk_initialise(subchunk, y >> 4);
        cubPaletteTuple to_add = { .block_id = block_id, .nb_blocks = 1 };
        cub_utils_list_append(subchunk->palette, &to_add);
        subchunk->blocks[x + (z << 4) + (y << 8)] = 1;
    } else {
        uint8_t* bp_id = &subchunk->blocks[x + (z << 4) + (y << 8)];
        if (*bp_id) // Not air => update block counter
            CUB_CHUNK_PALETTE(subchunk)[*bp_id - 1].nb_blocks--;
        *bp_id = cub_subchunk_palette_add(subchunk, block_id);
    }
}

void cub_chunk_render(cubChunk* chunk, cubBlockRenderer* block_renderer) {
    for (uint8_t i = 0; i < CUB_MAX_SUBCHUNKS; ++i) {
        cubSubChunk* subchunk = &chunk->subchunks[i];
        if (subchunk->y_pos >= CUB_MAX_SUBCHUNKS) continue;
        for (uint8_t y = 0; y < 16; ++y) {
            for (uint8_t z = 0; z < 16; ++z) {
                for (uint8_t x = 0; x < 16; ++x) {
                    uint8_t* bp_id = &subchunk->blocks[x + (z << 4) + (y << 8)];
                    if (!*bp_id) // Air => no rendering required
                        continue;
                    cub_block_render(block_renderer, NULL,
                            //CUB_CHUNK_PALETTE(subchunk)[*bp_id - 1].block_id,
                            CUB_VEC3(1.0f * (chunk->chunkX << 4) + 1.0f * x,
                                     1.0f * (subchunk->y_pos << 4) + 1.0f * y,
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

void cub_chunk_fill(cubChunk* chunk, cub_block_t id) {
    cubPaletteTuple to_add = { .block_id = id, .nb_blocks = 4096 };
    for (uint8_t i = 0; i < CUB_MAX_SUBCHUNKS; ++i) {
        cubSubChunk* subchunk = &chunk->subchunks[i];
        cub_subchunk_initialise(subchunk, i);
        memset(subchunk->blocks, 1, 4096 * sizeof(cub_block_t));
        cub_utils_list_append(subchunk->palette, &to_add);
    }
    chunk->nb_subchunks = CUB_MAX_SUBCHUNKS;
}
