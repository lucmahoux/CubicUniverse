#include "render/chunk.h"

void cub_chunk_load(cubChunk* chunk) {
    char path[20];
    sprintf(path, "%i.%i.chunk", chunk->chunkX, chunk->chunkZ);
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
            subchunk->palette = cub_utils_list(sizeof(cub_block_t));
            for (uint16_t i = 0; i < palette_len; ++i) {
                cub_block_t block_id;
                nb_read = fread(&block_id, sizeof(cub_block_t), 1, fp);
                if (nb_read != 1)
                    errx(1, "cub_chunk_load: Wrong format 'palette_id'!");
                cub_utils_list_append(subchunk->palette, &block_id);
            }
            // Block loading -> 4096 blocks per subchunk
            subchunk->blocks = malloc(4096 * sizeof(uint8_t));
            if (!subchunk->blocks)
                errx(1, "cub_chunk_load: Malloc failed :(!");
            for (uint16_t i = 0; i < 4096; ++i) {
                nb_read = fread(&subchunk->blocks[i], sizeof(uint8_t), 1, fp);
                if (nb_read != 1)
                    errx(1, "cub_chunk_load: Wrong format 'block_palette_id'!");
            }
        }
        fclose(fp);
    }
}

void cub_chunk_save(cubChunk* chunk) {
    char path[20];
    sprintf(path, "%i.%i.chunk", chunk->chunkX, chunk->chunkZ);
    printf("cub_chunk_save: Saving chunk at %s!", path);
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
            for (uint16_t i = 0; i < palette_len; ++i) {
                nb_written = fwrite(&subchunk->palette[i],
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
    return ((cub_block_t*)subchunk->palette->data)[block_palette_id - 1];
}

void cub_chunk_set_block(cubChunk* chunk, uint8_t x, uint8_t y,
                         uint8_t z, cub_block_t block_id) {
    cubSubChunk* subchunk = &chunk->subchunks[y >> 4];
    if (block_id == 0) { // Replace block by air
        if (subchunk->y_pos >= CUB_MAX_SUBCHUNKS)
            return;
        subchunk->blocks[x + (z << 4) + (y << 8)] = 0;
    } else if (subchunk->y_pos >= CUB_MAX_SUBCHUNKS) { // New subchunk
        subchunk->y_pos = y >> 4;
        subchunk->palette = cub_utils_list(sizeof(cub_block_t));
        subchunk->blocks = malloc(4096 * sizeof(uint8_t));
        if (!subchunk->blocks)
            errx(1, "cub_chunk_set_block: Malloc failed!");
        memset(subchunk->blocks, 0, 4096 * sizeof(uint8_t));
        cub_utils_list_append(subchunk->palette, &block_id);
        subchunk->blocks[x + (z << 4) + (y << 8)] = 1;
    } else {
        uint8_t block_palette_id = 0;
        cubList* palette = subchunk->palette;
        for (; block_palette_id < palette->len; ++block_palette_id) {
            cub_block_t id = ((cub_block_t*)palette->data)[block_palette_id];
            if (id == block_id) break;
        }
        if (block_palette_id >= palette->len) {
            if (block_palette_id == 255) {
                printf("Cannot add any more block with a different id, "
                        "palette is full!\n");
                return;
            }
            cub_utils_list_append(palette, &block_id);
        }
        subchunk->blocks[x + (z << 4) + (y << 8)] = block_palette_id + 1;
    }
}

void cub_chunk_render(cubChunk* chunk, cubBlockRenderer* block_renderer) {
    for (uint8_t i = 0; i < CUB_MAX_SUBCHUNKS; ++i) {
        cubSubChunk* subchunk = &chunk->subchunks[i];
        if (subchunk->y_pos >= CUB_MAX_SUBCHUNKS) continue;
        for (uint8_t y = 0; y < 16; ++y)
            for (uint8_t z = 0; z < 16; ++z)
                for (uint8_t x = 0; x < 16; ++x)
                    cub_block_render(block_renderer,
                            ((cub_block_t*)subchunk->palette->data)
                            [subchunk->blocks[x + (z << 4) + (y << 8)]],
                            CUB_VEC3(1.0f * (chunk->chunkX << 4) + 1.0f * x,
                                     1.0f * (subchunk->y_pos << 4) + 1.0f * y,
                                     1.0f * (chunk->chunkZ << 4) + 1.0f * z));
    }
}
