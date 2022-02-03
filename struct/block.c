#include "struct/block.h"

void cub_block_load_block_list() {
    printf("Trying to load %s...\n", CUB_BLOCK_LIST_PATH);
    FILE* fp;
    if ( (fp = fopen(CUB_BLOCK_LIST_PATH, "r")) == NULL ) {
        errx(1, "cub_block_load_block_list: File not found: %s!",
                CUB_BLOCK_LIST_PATH);
    } else {
        char buffer[CUB_BLOCK_FILE_BUFFER];
        size_t nb_read = fread( buffer, sizeof(uint32_t),
                                CUB_BLOCK_FILE_BUFFER, fp);
        if (nb_read != sizeof(uint32_t))
            errx(1, "cub_block_load_block_list: Error - wrong format!");
        uint32_t* nb_blocks = (uint32_t*) buffer;
        CUB_ALL_BLOCKS.blocks = malloc(*nb_blocks * sizeof(cubBlockData));
        if (!CUB_ALL_BLOCKS.blocks)
            errx(1, "cub_block_load_block_list: Malloc failed!");
        CUB_ALL_BLOCKS.nb_blocks = *nb_blocks;
        size_t name_len = sizeof(CUB_ALL_BLOCKS.blocks[0].name);
        size_t id_len = sizeof(CUB_ALL_BLOCKS.blocks[0].id);
        size_t type_len = sizeof(CUB_ALL_BLOCKS.blocks[0].type);
        for (uint32_t i = 0; i < *nb_blocks; ++i) {
            cubBlockData* current = &CUB_ALL_BLOCKS.blocks[i];
            // Load block name
            nb_read = fread(buffer, name_len, CUB_BLOCK_FILE_BUFFER, fp);
            if (nb_read != name_len)
                errx(1, "cub_block_load_block_list: Incorrect 'name' format!");
            memcpy(current->name, buffer, name_len);

            // Load block id
            nb_read = fread(buffer, id_len, CUB_BLOCK_FILE_BUFFER, fp);
            if (nb_read != id_len)
                errx(1, "cub_block_load_block_list: Incorrect 'id' format!");
            memcpy(&current->id, buffer, id_len);

            // Load block type
            nb_read = fread(buffer, type_len, CUB_BLOCK_FILE_BUFFER, fp);
            if (nb_read != type_len)
                errx(1, "cub_block_load_block_list: Incorrect 'type' format!");
            memcpy(&current->type, buffer, type_len);

            // Set up memory allocations for textures identifiers
            switch (current->type) {
                case CUBE_SAME_FACES:
                    current->textures_id = malloc(sizeof(GLuint));
                    if (!current->textures_id)
                        errx(1, "cub_block_load_block_list: Malloc failed!");
                    break;
                case CUBE_TOP_BOTTOM:
                    current->textures_id = malloc(3 * sizeof(GLuint));
                    if (!current->textures_id)
                        errx(1, "cub_block_load_block_list: Malloc failed!");
                    break;
                case CUBE_UNIQUE_FACES:
                    current->textures_id = malloc(6 * sizeof(GLuint));
                    if (!current->textures_id)
                        errx(1, "cub_block_load_block_list: Malloc failed!");
                    break;
                default:
                    errx(1, "cub_block_load_block_list: Unknown block type!");
            }
        }
        printf("Block list loaded successfully!\n");
    }
}

void cub_block_load_texture_same_faces(char* buffer, size_t name_len,
                                        GLuint* tex_id_block) {
    char* start = buffer + name_len;
    strcpy(start, ".png");
    cub_graphics_bind_load_texture(tex_id_block, buffer);
}

void cub_block_load_texture_top_bottom_faces(char* buffer, size_t name_len,
                                             GLuint* tex_id_block) {
    char* start = buffer + name_len;
    strcpy(start, "top.png");
    cub_graphics_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
    strcpy(start, "bottom.png");
    cub_graphics_bind_load_texture(tex_id_block, buffer);
}

// The buffer must have a sufficient allocated memory to handle the changes made
void cub_block_load_texture_unique_faces(char* buffer, size_t name_len,
                                         GLuint* tex_id_block) {
    char* start = buffer + name_len;
    strcpy(start, "east.png");
    cub_graphics_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
    strcpy(start, "north.png");
    cub_graphics_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
    strcpy(start, "west.png");
    cub_graphics_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
    strcpy(start, "south.png");
    cub_graphics_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
    cub_block_load_texture_same_faces(buffer, name_len, tex_id_block);
}

void cub_block_load_texture_pack() {
    char buffer[CUB_BLOCK_FILE_BUFFER * 2];
    for (size_t i = 0; i < CUB_ALL_BLOCKS.nb_blocks; ++i) {
        cubBlockData* current = &CUB_ALL_BLOCKS.blocks[i];
        size_t name_len = strlen(current->name) + 1;
        strcpy(buffer, current->name);
        buffer[name_len - 1] = '_';
        switch (current->type) {
            case CUBE_SAME_FACES:
                glGenTextures(1, current->textures_id);
                cub_block_load_texture_same_faces(buffer, name_len,
                                                  current->textures_id);
                break;
            case CUBE_TOP_BOTTOM:
                glGenTextures(3, current->textures_id);
                cub_block_load_texture_top_bottom_faces(buffer, name_len,
                                                        current->textures_id);
                break;
            case CUBE_UNIQUE_FACES:
                glGenTextures(6, current->textures_id);
                cub_block_load_texture_unique_faces(buffer, name_len,
                                                    current->textures_id);
                break;
        }
    }
}

void cub_block_free_all_textures() {
    for (size_t i = 0; i < CUB_ALL_BLOCKS.nb_blocks; ++i) {
        cubBlockData* current = &CUB_ALL_BLOCKS.blocks[i];
        switch (current->type) {
            case CUBE_SAME_FACES:
                glDeleteTextures(1, current->textures_id);
                break;
            case CUBE_TOP_BOTTOM:
                glDeleteTextures(3, current->textures_id);
                break;
            case CUBE_UNIQUE_FACES:
                glDeleteTextures(6, current->textures_id);
                break;
        }
    }
}
