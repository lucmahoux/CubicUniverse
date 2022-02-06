#include "render/block.h"

void cub_block_setup_renderer(cubBlockRenderer* renderer) {
    printf("cub_block_setup_renderer: Starting renderer initialisation...\n");
    const GLfloat block_vertices[] = {
        0.0f,   0.0f,   1.0f,   0.0f,   0.0f,   1.0f,   // Face 1 - South
        1.0f,   0.0f,   1.0f,   1.0f,   0.0f,   1.0f,
        1.0f,   1.0f,   1.0f,   1.0f,   1.0f,   1.0f,
        0.0f,   1.0f,   1.0f,   0.0f,   1.0f,   1.0f,

        1.0f,   0.0f,   1.0f,   0.0f,   0.0f,   2.0f,   // Face 2 - East
        1.0f,   0.0f,   0.0f,   1.0f,   0.0f,   2.0f,
        1.0f,   1.0f,   0.0f,   1.0f,   1.0f,   2.0f,
        1.0f,   1.0f,   1.0f,   0.0f,   1.0f,   2.0f,

        1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   3.0f,   // Face 3 - North
        0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   3.0f,
        0.0f,   1.0f,   0.0f,   1.0f,   1.0f,   3.0f,
        1.0f,   1.0f,   0.0f,   0.0f,   1.0f,   3.0f,

        0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   // Face 4 - West
        0.0f,   0.0f,   1.0f,   1.0f,   0.0f,   4.0f,
        0.0f,   1.0f,   1.0f,   1.0f,   1.0f,   4.0f,
        0.0f,   1.0f,   0.0f,   0.0f,   1.0f,   4.0f,

        0.0f,   1.0f,   1.0f,   0.0f,   0.0f,   5.0f,   // Face 5 - Top
        1.0f,   1.0f,   1.0f,   1.0f,   0.0f,   5.0f,
        1.0f,   1.0f,   0.0f,   1.0f,   1.0f,   5.0f,
        0.0f,   1.0f,   0.0f,   0.0f,   1.0f,   5.0f,

        0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   6.0f,   // Face 6 - Bottom
        1.0f,   0.0f,   0.0f,   1.0f,   0.0f,   6.0f,
        1.0f,   0.0f,   1.0f,   1.0f,   1.0f,   6.0f,
        0.0f,   0.0f,   1.0f,   0.0f,   1.0f,   6.0f
    };
    const GLuint block_indices[] = {
        0,  1,  2,  0,  2,  3,  // Face 1
        4,  5,  6,  4,  6,  7,  // Face 2
        8,  9,  10, 8,  10, 11, // Face 3
        12, 13, 14, 12, 14, 15, // Face 4
        16, 17, 18, 16, 18, 19, // Face 5
        20, 21, 22, 20, 22, 23  // Face 6
    };

    // Generate the buffers
    GLuint buffers[2];
    GLuint VAO;
    glGenBuffers(2, buffers);
    glGenVertexArrays(1, &VAO);

    // Bind the vertex array object then set up the other buffers data
    glBindVertexArray(VAO);
    // 1) Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(block_vertices),
                 block_vertices, GL_STATIC_DRAW);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(block_indices),
                 block_indices, GL_STATIC_DRAW);

    // Unbind everything [!] Order of unbinding matters [!]
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    renderer->VAO = VAO;
    renderer->VBO = buffers[0];
    renderer->EBO = buffers[1];

    // Load the shader program
    renderer->shader_program = cub_utils_build_shader("block");

    // Get uniforms locations (for textures)
    GLint loc = glGetUniformLocation(renderer->shader_program, "tex_south");
    if (loc == -1)
        errx(1, "cub_block_setup_renderer: tex_south uniform is missing!");
    renderer->tex_locations[0] = loc;
    loc = glGetUniformLocation(renderer->shader_program, "tex_east");
    if (loc == -1)
        errx(1, "cub_block_setup_renderer: tex_east uniform is missing!");
    renderer->tex_locations[1] = loc;
    loc = glGetUniformLocation(renderer->shader_program, "tex_north");
    if (loc == -1)
        errx(1, "cub_block_setup_renderer: tex_north uniform is missing!");
    renderer->tex_locations[2] = loc;
    loc = glGetUniformLocation(renderer->shader_program, "tex_west");
    if (loc == -1)
        errx(1, "cub_block_setup_renderer: tex_west uniform is missing!");
    renderer->tex_locations[3] = loc;
    loc = glGetUniformLocation(renderer->shader_program, "tex_top");
    if (loc == -1)
        errx(1, "cub_block_setup_renderer: tex_top uniform is missing!");
    renderer->tex_locations[4] = loc;
    loc = glGetUniformLocation(renderer->shader_program, "tex_bottom");
    if (loc == -1)
        errx(1, "cub_block_setup_renderer: tex_bottom uniform is missing!");
    renderer->tex_locations[5] = loc;

    // Get the uniform value of the model matrix
    renderer->model_uni_loc = glGetUniformLocation(renderer->shader_program,
                                                    "model_matrix");
    renderer->model_matrix = cub_utils_mat4(1.0f);

    // Load all the blocks
    cub_block_load_block_list(&renderer->block_list);
    printf("cub_block_setup_renderer: Renderer initialised!\n");

    // Enable depth testing, otherwise weird rendering of cubes
    glEnable(GL_DEPTH_TEST);
    // Accept the fragment if closer to the camera than the former one
    glDepthFunc(GL_LESS);
}

void cub_block_render(cubBlockRenderer* renderer, cub_block_t id,
                        cubVec3 position) {
    cubBlockData* b_data = &renderer->block_list.blocks[id];

    switch (b_data->type) {
        case CUBE_SAME_FACES:
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, b_data->textures_id[0]);
            for (uint8_t i = 0; i < 6; ++i)
                glUniform1i(renderer->tex_locations[i], 0);
            break;
        case CUBE_TOP_BOTTOM:
            glActiveTexture(GL_TEXTURE0); // Top
            glBindTexture(GL_TEXTURE_2D, b_data->textures_id[0]);
            glActiveTexture(GL_TEXTURE1); // Bottom
            glBindTexture(GL_TEXTURE_2D, b_data->textures_id[1]);
            glActiveTexture(GL_TEXTURE2); // Sides
            glBindTexture(GL_TEXTURE_2D, b_data->textures_id[2]);
            for (uint8_t i = 0; i < 4; ++i)
                glUniform1i(renderer->tex_locations[i], 2);
            glUniform1i(renderer->tex_locations[4], 0);
            glUniform1i(renderer->tex_locations[5], 1);
            break;
        case CUBE_UNIQUE_FACES:
            for (uint8_t i = 0; i < 6; ++i) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, b_data->textures_id[i]);
                glUniform1i(renderer->tex_locations[i], i);
            }
            break;
    }

    cubMat4 new_model = CUB_MAT4_TRANS(renderer->model_matrix, position);
    glUniformMatrix4fv(renderer->model_uni_loc, 1, GL_FALSE, new_model.coeffs);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*) 0);
}

void cub_block_free_renderer(cubBlockRenderer* renderer) {
    cub_block_free_all_textures(&renderer->block_list);
    free(renderer->block_list.blocks);
    glDeleteBuffers(1, &renderer->VBO);
    glDeleteBuffers(1, &renderer->EBO);
    glDeleteVertexArrays(1, &renderer->VAO);
    glDeleteProgram(renderer->shader_program);
}

void cub_block_load_block_list(cubBlockList* block_list) {
    char* fpath = cub_utils_strconcat(GAMEFILES_PATH, BLOCK_LIST_FILE, NULL);
    printf("cub_block_load_block_list: Trying to load %s...\n", fpath);
    FILE* fp;
    if ( (fp = fopen(fpath, "rb")) == NULL ) {
        errx(1, "cub_block_load_block_list: File not found: %s!", fpath);
    } else {
        size_t nb_read;
        uint32_t nb_blocks;
        nb_read = fread(&nb_blocks, sizeof(uint32_t), 1, fp);
        if (nb_read != 1)
            errx(1, "cub_block_load_block_list: Error - wrong format!");
        block_list->blocks = malloc(nb_blocks * sizeof(cubBlockData));
        if (!block_list->blocks)
            errx(1, "cub_block_load_block_list: Malloc failed!");
        block_list->nb_blocks = nb_blocks;
        size_t name_len = CUB_MAX_BLOCK_STRLEN;
        size_t id_len = sizeof(cub_block_t);
        size_t type_len = sizeof(cubBlockTextureType);
        for (uint32_t i = 0; i < nb_blocks; ++i) {
            cubBlockData* current = &block_list->blocks[i];
            // Load block name
            nb_read = fread(&current->name, sizeof(char), name_len, fp);
            if (nb_read != name_len)
                errx(1, "cub_block_load_block_list: Incorrect 'name' format!");

            // Load block id
            nb_read = fread(&current->id, id_len, 1, fp);
            if (nb_read != 1)
                errx(1, "cub_block_load_block_list: Incorrect 'id' format!");

            // Load block type
            nb_read = fread(&current->type, type_len, 1, fp);
            if (nb_read != 1)
                errx(1, "cub_block_load_block_list: Incorrect 'type' format!");

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
        printf("cub_block_load_block_list: Block list loaded successfully!\n");
    }
    free(fpath);
    fclose(fp);
}

void cub_block_load_texture_same_faces(char* buffer, size_t name_len,
                                        GLuint* tex_id_block) {
    char* start = buffer + name_len - 1;
    strcpy(start, ".png");
    cub_utils_bind_load_texture(tex_id_block, buffer);
}

void cub_block_load_texture_top_bottom_faces(char* buffer, size_t name_len,
                                             GLuint* tex_id_block) {
    char* start = buffer + name_len;
    strcpy(start, "top.png");
    cub_utils_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
    strcpy(start, "bottom.png");
    cub_utils_bind_load_texture(tex_id_block, buffer);
}

// The buffer must have a sufficient allocated memory to handle the changes made
void cub_block_load_texture_unique_faces(char* buffer, size_t name_len,
                                         GLuint* tex_id_block) {
    char* start = buffer + name_len;
    strcpy(start, "south.png");
    cub_utils_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
    strcpy(start, "east.png");
    cub_utils_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
    strcpy(start, "north.png");
    cub_utils_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
    strcpy(start, "west.png");
    cub_utils_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
    strcpy(start, "top.png");
    cub_utils_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
    strcpy(start, "bottom.png");
    cub_utils_bind_load_texture(tex_id_block, buffer);
    ++tex_id_block;
}

void cub_block_load_texture_pack(cubBlockList* block_list) {
    printf("cub_block_load_texture_pack: Starting to load texture pack...\n");
    char buffer[CUB_MAX_BLOCK_STRLEN * 2];
    for (size_t i = 0; i < block_list->nb_blocks; ++i) {
        cubBlockData* current = &block_list->blocks[i];
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
    printf("cub_block_load_texture_pack: texture pack loaded!\n");
}

void cub_block_free_all_textures(cubBlockList* block_list) {
    printf("cub_block_free_all_textures: Starting to free textures...\n");
    for (size_t i = 0; i < block_list->nb_blocks; ++i) {
        cubBlockData* current = &block_list->blocks[i];
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
        free(current->textures_id);
    }
    printf("cub_block_free_all_textures: Textures freed!\n");
}

void cub_block_new_block(const char* name, cub_block_t id,
                            cubBlockTextureType type) {
    char* fpath = cub_utils_strconcat(GAMEFILES_PATH, BLOCK_LIST_FILE, NULL);
    FILE* fp;
    if ( (fp = fopen(fpath, "r+b")) != NULL ) {
        uint32_t block_counter = 0;
        size_t nb_written;
        if (fread(&block_counter, sizeof(uint32_t), 1, fp) != 1) { // New file
            rewind(fp);
            nb_written = fwrite(&block_counter, sizeof(uint32_t), 1, fp);
            if (nb_written != 1)
                errx(1, "cub_block_new_block: fwrite(), writing block counter");
        }
        fseek(fp, 0, SEEK_END);
        char buffer[CUB_MAX_BLOCK_STRLEN];
        size_t i = 0;
        for (; name[i] && i < CUB_MAX_BLOCK_STRLEN; ++i)
            buffer[i] = name[i];
        for(; i < CUB_MAX_BLOCK_STRLEN; ++i)
            buffer[i] = '\0';
        nb_written = fwrite(buffer, sizeof(char), CUB_MAX_BLOCK_STRLEN, fp);
        if (nb_written != CUB_MAX_BLOCK_STRLEN)
            errx(1, "cub_block_new_block: fwrite(), writing block name failed");
        nb_written = fwrite(&id, sizeof(cub_block_t), 1, fp);
        if (nb_written != 1)
            errx(1, "cub_block_new_block: fwrite(), writing block id failed");
        nb_written = fwrite(&type, sizeof(cubBlockTextureType), 1, fp);
        if (nb_written != 1)
            errx(1, "cub_block_new_block: fwrite(), writing block type failed");
        // Update the block counter
        ++block_counter;
        fseek(fp, 0, SEEK_SET);
        nb_written = fwrite(&block_counter, sizeof(uint32_t), 1, fp);
        if (nb_written != 1)
            errx(1, "cub_block_new_block: fwrite(), writing block counter");
    } else errx(1, "cub_block_new_block: failed to open %s!\
                    Please create it so that it can be populated!", fpath);
    free(fpath);
    fclose(fp);
}
