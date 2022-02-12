#include "render/block_list_loader.h"

uint8_t cub_BLloader_count_draw_tex(cubRenderType type) {
    uint8_t count = 0;
    if (type & RT_DEFAULT) ++count;
    if (type & RT_TOP) ++count;
    if (type & RT_BOTTOM) ++count;
    if (type & RT_SIDE) ++count;
    if (type & RT_FRONT) ++count;
    if (type & RT_BACK) ++count;
    if (type & RT_LEFT) ++count;
    if (type & RT_RIGHT) ++count;
    return count;
}

void cub_BLloader_allocate_tex_mem(cubBlockData* block) {
    uint16_t nb_modes = block->block_info.has_bs_tex
                ? cub_BS_TEX_get_nb_modes(BS_TEX(block))
                : 1;
    block->textures = malloc(nb_modes * block->nb_tex_draw * sizeof(GLuint));
    if (!block->textures)
        errx(1, "cub_allocate_textures_mem: Malloc failed!");
    printf("Allocated %u GLuint for block ID: %i!\n",
            nb_modes * block->nb_tex_draw, block->id);
}

void cub_BLloader_create_VAO(GLfloat* VBO, GLuint* EBO,
                             GLsizei VBO_len, GLsizei EBO_len,
                             cubRenderBufferObject* render_obj) {
    // Generate the buffers
    glGenBuffers(1, &render_obj->VBO);
    glGenBuffers(1, &render_obj->EBO);
    glGenVertexArrays(1, &render_obj->VAO);

    // Bind the vertex array object then set up the other buffers data
    glBindVertexArray(render_obj->VAO);
    // 1) Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, render_obj->VBO);
    glBufferData(GL_ARRAY_BUFFER, VBO_len * sizeof(GLfloat),
                 VBO, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        6 * sizeof(GLfloat), (GLvoid*) 0);
    glEnableVertexAttribArray(0); // Coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                        6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1); // Texture Coords
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
                        6 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2); // Face identifier (NSEWTB)
    // 2) Element Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_obj->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, EBO_len * sizeof(GLuint),
                 EBO, GL_STATIC_DRAW);
    render_obj->nb_elements = EBO_len;
    printf("nb_elements = %u - VBO_last = %f - EBO_last = %u\n",
            EBO_len, VBO[VBO_len - 1], EBO[EBO_len - 1]);

    // Unbind everything [!] Order of unbinding matters [!]
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void cub_BLloader_load_header(cubBlockList* block_list, uint8_t* nb_buffers,
                                cubRenderBufferObject** render_objs, FILE* fp) {
    const char fname[] = "cub_BLloader_load";
    // Get the number of rendering buffers & the number of blocks
    cub_utils_fread(nb_buffers, sizeof(uint8_t), 1,
                    fp, fname, "nb_buffers");
    cub_utils_fread(&block_list->nb_blocks, sizeof(cub_block_t), 1,
                    fp, fname, "nb_blocks");
    // Allocate the arrays of buffer & cubBlockData accordingly
    *render_objs = malloc(*nb_buffers * sizeof(cubRenderBufferObject));
    block_list->blocks = malloc(block_list->nb_blocks * sizeof(cubBlockData));
    if (!*render_objs || !block_list->blocks)
        errx(1, "cub_BLloader_load_header: Malloc failed!");
    for (uint16_t i = 0; i < *nb_buffers; ++i) {
        GLsizei VBO_len;
        cub_utils_fread(&VBO_len, sizeof(GLsizei), 1, fp, fname, "VBO_len");
        GLfloat VBO[VBO_len];
        cub_utils_fread(&VBO, sizeof(GLfloat), VBO_len, fp, fname, "VBO");
        GLsizei EBO_len;
        cub_utils_fread(&EBO_len, sizeof(GLsizei), 1, fp, fname, "EBO_len");
        GLuint EBO[EBO_len];
        cub_utils_fread(&EBO, sizeof(GLuint), EBO_len, fp, fname, "EBO");
        cub_BLloader_create_VAO(VBO, EBO, VBO_len, EBO_len, *render_objs + i);
    }
}

void cub_BLloader_load_blocks(cubBlockList* block_list, FILE* fp) {
    const char fname[] = "cub_BLloader_load_blocks";
    for (uint32_t i = 0; i < block_list->nb_blocks; ++i) {
        cubBlockData* current = &block_list->blocks[i];
        cub_utils_fread(&current->name, sizeof(char), CUB_MAX_BLOCK_STRLEN,
                        fp, fname, "name"); // Name
        cub_utils_fread(&current->id, sizeof(cub_block_t), 1,
                        fp, fname, "id"); // ID
        cub_utils_fread(&current->render_type, sizeof(cubRenderType), 1,
                        fp, fname, "render_type"); // Render type
        current->nb_tex_draw =
                        cub_BLloader_count_draw_tex(current->render_type);
        cub_utils_fread(&current->VAO_id, sizeof(uint8_t), 1,
                        fp, fname, "VAO_ID"); // VAO ID
        cub_utils_fread(&current->block_info, sizeof(cubBlockInfo), 1,
                        fp, fname, "block_info"); // Block info
        // Set up blockstates (if any)
        if (current->block_info.has_states) {
            uint8_t nb_bs = current->block_info.nb_states;
            current->bs_keys = malloc(nb_bs * sizeof(cub_bs_key));
            current->bs_default_values = malloc(nb_bs * sizeof(cub_bs_val));
            if (!current->bs_keys || !current->bs_default_values)
                errx(1, "Malloc failed!");
            for (uint8_t k = 0; k < nb_bs; ++k) {
                cub_utils_fread(&current->bs_keys[k], sizeof(cub_bs_key), 1,
                                fp, fname, "blockstate_key");
                cub_utils_fread(&current->bs_default_values[k],
                                sizeof(cub_bs_val), 1, fp, fname,
                                    "blockstate_default_value");
            }
            if (current->block_info.has_bs_tex) {
                current->bs_name_parser = current->block_info.is_bs_creator
                                ? cub_BS_is_creator(BS_TEX(current))
                                : cub_BS_is_modificator(BS_TEX(current));
            } else current->bs_name_parser = NULL;
        } else {
            current->bs_keys = NULL;
            current->bs_default_values = NULL;
            current->bs_name_parser = NULL;
        }
        // Allocate memory for the textures
        cub_BLloader_allocate_tex_mem(current);
    }
}

void cub_BLloader_load(cubBlockList* block_list, uint8_t* nb_buffers,
                        cubRenderBufferObject** render_objs) {
    char* fpath = cub_utils_strconcat(GAMEFILES_PATH, BLOCK_LIST_FILE, NULL);
    printf("cub_block_load_block_list: Trying to load %s...\n", fpath);
    FILE* fp;
    if ( (fp = fopen(fpath, "rb")) == NULL ) {
        errx(1, "cub_block_load_block_list: File not found: %s!", fpath);
    } else {
        cub_BLloader_load_header(block_list, nb_buffers, render_objs, fp);
        cub_BLloader_load_blocks(block_list, fp);
        free(fpath);
        fclose(fp);
        printf("cub_block_load_block_list: Block list loaded successfully!\n");
    }
}

void cub_BLloader_create_block_list_file() {
    char* fpath = cub_utils_strconcat(GAMEFILES_PATH, BLOCK_LIST_FILE, NULL);
    FILE* fp = fopen(fpath, "w");
    if (fp) fclose(fp);
    free(fpath);
}

void cub_BLloader_add_buffer(GLfloat* VBO, GLsizei VBO_len,
                             GLuint* EBO, GLsizei EBO_len) {
    char* fpath = cub_utils_strconcat(GAMEFILES_PATH, BLOCK_LIST_FILE, NULL);
    FILE* fp;
    if ( (fp = fopen(fpath, "r+b")) != NULL ) {
        const char fname[] = "cub_BLloader_add_buffer";
        uint8_t nb_buffers = 0;
        if (fread(&nb_buffers, sizeof(uint8_t), 1, fp) != 1) {
            // New file
            rewind(fp);
            cub_utils_fwrite(&nb_buffers, sizeof(uint8_t), 1,
                             fp, fname, "nb_buffers");
            cub_block_t nb_blocks = 0;
            cub_utils_fwrite(&nb_blocks, sizeof(cub_block_t), 1,
                             fp, fname, "nb_blocks");
        }
        fseek(fp, 0, SEEK_END); // Go to the end of the file
        cub_utils_fwrite(&VBO_len, sizeof(GLsizei), 1,
                         fp, fname, "VBO_len");
        cub_utils_fwrite(VBO, sizeof(GLfloat), VBO_len,
                         fp, fname, "VBO");
        cub_utils_fwrite(&EBO_len, sizeof(GLsizei), 1,
                         fp, fname, "EBO_len");
        cub_utils_fwrite(EBO, sizeof(GLuint), EBO_len,
                         fp, fname, "EBO");
        fseek(fp, 0, SEEK_SET); // Go to the start of the file
        ++nb_buffers; // Update the buffer counter
        cub_utils_fwrite(&nb_buffers, sizeof(uint8_t), 1,
                         fp, fname, "nb_buffers");
        fclose(fp);
    } else errx(1, "cub_BLloader_add_buffer: failed to open %s!\
                    Please create it so that it can be populated!", fpath);
    free(fpath);
}

void cub_BLloader_add_block(const char* name, cubRenderType render_type,
                            uint8_t VAO_id, cubBlockInfo block_info,
                            cub_bs_key* bs_keys,
                            cub_bs_val* bs_default_values) {
    char* fpath = cub_utils_strconcat(GAMEFILES_PATH, BLOCK_LIST_FILE, NULL);
    FILE* fp;
    if ( (fp = fopen(fpath, "r+b")) != NULL ) {
        const char fname[] = "cub_BLloader_add_block";
        fseek(fp, sizeof(uint8_t), SEEK_SET);
        cub_block_t nb_blocks;
        cub_utils_fread(&nb_blocks, sizeof(cub_block_t), 1,
                        fp, fname, "nb_blocks");
        fseek(fp, 0, SEEK_END);
        char buffer[CUB_MAX_BLOCK_STRLEN];
        for (uint8_t i = 0; i < CUB_MAX_BLOCK_STRLEN; ++i)
            buffer[i] = name[i];
        cub_utils_fwrite(buffer, sizeof(char), CUB_MAX_BLOCK_STRLEN,
                         fp, fname, "block_name");
        // Block ID is the previous number of blocks in the list
        cub_utils_fwrite(&nb_blocks, sizeof(cub_block_t), 1,
                         fp, fname, "block_id");
        cub_utils_fwrite(&render_type, sizeof(cubRenderType), 1,
                         fp, fname, "block_render_type");
        cub_utils_fwrite(&VAO_id, sizeof(uint8_t), 1,
                         fp, fname, "block_VAO_id");
        cub_utils_fwrite(&block_info, sizeof(cubBlockInfo), 1,
                         fp, fname, "block_info");
        for (uint8_t i = 0; i < block_info.nb_states; ++i) {
            cub_utils_fwrite(bs_keys + i, sizeof(cub_bs_key), 1,
                             fp, fname, "blockstate_key");
            cub_utils_fwrite(bs_default_values + i, sizeof(cub_bs_val), 1,
                             fp, fname, "blockstate_value");
        }
        fseek(fp, sizeof(uint8_t), SEEK_SET);
        ++nb_blocks; // Update the block counter
        cub_utils_fwrite(&nb_blocks, sizeof(cub_block_t), 1,
                         fp, fname, "nb_blocks");
        fclose(fp);
    } else errx(1, "cub_BLloader_add_block: failed to open %s!\
                    Please create it so that it can be populated!", fpath);
    free(fpath);
}
