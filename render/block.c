#include "render/block.h"

void block_setup_renderer(blockRenderer* renderer) {
    printf("block_setup_renderer: Starting renderer initialisation...\n");

    // Load the shader program
    renderer->shader_program = build_shader("block");

    // Get uniforms locations (for textures)
    GLint loc = glGetUniformLocation(renderer->shader_program, "tex_top");
    if (loc == -1)
        errx(1, "block_setup_renderer: tex_top uniform is missing!");
    renderer->tex_locations[CUB_TOP_ID] = loc;
    loc = glGetUniformLocation(renderer->shader_program, "tex_bottom");
    if (loc == -1)
        errx(1, "block_setup_renderer: tex_bottom uniform is missing!");
    renderer->tex_locations[CUB_BOTTOM_ID] = loc;
    loc = glGetUniformLocation(renderer->shader_program, "tex_front");
    if (loc == -1)
        errx(1, "block_setup_renderer: tex_front uniform is missing!");
    renderer->tex_locations[CUB_FRONT_ID] = loc;
    loc = glGetUniformLocation(renderer->shader_program, "tex_back");
    if (loc == -1)
        errx(1, "block_setup_renderer: tex_back uniform is missing!");
    renderer->tex_locations[CUB_BACK_ID] = loc;
    loc = glGetUniformLocation(renderer->shader_program, "tex_left");
    if (loc == -1)
        errx(1, "block_setup_renderer: tex_left uniform is missing!");
    renderer->tex_locations[CUB_LEFT_ID] = loc;
    loc = glGetUniformLocation(renderer->shader_program, "tex_right");
    if (loc == -1)
        errx(1, "block_setup_renderer: tex_right uniform is missing!");
    renderer->tex_locations[CUB_RIGHT_ID] = loc;

    // Get the uniform value of the model matrix
    renderer->model_uni_loc = glGetUniformLocation(renderer->shader_program,
                                                    "model_matrix");
    renderer->model_matrix = MAT4(1.0f);

    // Load all the blocks & the rendering buffers
    BLloader_load(&renderer->block_list, &renderer->nb_buffers,
                        &renderer->buffer_objs);

    // Enable depth testing, otherwise weird rendering of cubes
    glEnable(GL_DEPTH_TEST);
    // Accept the fragment if closer to the camera than the former one
    glDepthFunc(GL_LESS);

    printf("block_setup_renderer: Renderer initialised!\n");
}

void block_update_tex_uniform(GLuint tex_loc, uint8_t tex_id,
                                    uint8_t gl_tex_nbr) {
    glActiveTexture(GL_TEXTURE0 + gl_tex_nbr);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glUniform1i(tex_loc, gl_tex_nbr);
}

void block_set_tex_default(blockRenderer* renderer, blockData* block,
                                blockState* bs) {
    GLuint tex_ids[6] = { 0, 0, 0, 0, 0, 0 };
    uint8_t tex_id_block = (block->block_info.has_bs_tex
                            ? bs->states[BS_TEX_ID]
                            : 0) * block->nb_tex_draw;
    if (block->render_type & RT_TOP)
        tex_ids[CUB_TOP_ID] = block->textures[tex_id_block++];
    if (block->render_type & RT_BOTTOM)
        tex_ids[CUB_BOTTOM_ID] = block->textures[tex_id_block++];
    if (block->render_type & RT_FRONT)
        tex_ids[CUB_FRONT_ID] = block->textures[tex_id_block++];
    if (block->render_type & RT_BACK)
        tex_ids[CUB_BACK_ID] = block->textures[tex_id_block++];
    if (block->render_type & RT_LEFT)
        tex_ids[CUB_LEFT_ID] = block->textures[tex_id_block++];
    if (block->render_type & RT_RIGHT)
        tex_ids[CUB_RIGHT_ID] = block->textures[tex_id_block++];
    if (block->render_type & RT_SIDE) {
        for (uint8_t i = CUB_SIDE_START_ID; i <= CUB_SIDE_END_ID; ++i)
            if (!tex_ids[i])
                tex_ids[i] = block->textures[tex_id_block];
        ++tex_id_block;
    }
    // Top & Bottom textures are complementary
    if (tex_ids[CUB_TOP_ID] && !tex_ids[CUB_BOTTOM_ID])
        tex_ids[CUB_BOTTOM_ID] = tex_ids[CUB_TOP_ID];
    if (block->render_type & RT_DEFAULT) {
        for (uint8_t i = CUB_DEFAULT_START_ID; i <= CUB_DEFAULT_END_ID; ++i)
            if (!tex_ids[i])
                tex_ids[i] = block->textures[tex_id_block];
    }
    for (uint8_t i = 0; i < 6; ++i)
        block_update_tex_uniform(renderer->tex_locations[i], tex_ids[i], i);
}

void block_set_tex_custom(blockRenderer* renderer, blockData* block,
                                blockState* bs) {
    uint8_t offset = (block->block_info.has_bs_tex
                      ? bs->states[BS_TEX_ID]
                      : 0) * block->nb_tex_draw;
    for (uint8_t i = 0; i < block->nb_tex_draw; ++i)
        block_update_tex_uniform(renderer->tex_locations[i],
                                     block->textures[offset + i], i);
}

void block_render(blockRenderer* renderer, blockState* bs,
                        vec3 position) {
    blockData* block = CUB_BLOCK_DATA(renderer, bs->id);
    renderBufferObject* r_obj = &renderer->buffer_objs[block->VAO_id];
    if (block->VAO_id != DEFAULT_VAO_ID) {
        glBindVertexArray(r_obj->VAO);
        block_set_tex_custom(renderer, block, bs);
    } else
        block_set_tex_default(renderer, block, bs);

    // TODO: Apply BS transformations (rotations, ...)
    mat4 new_model = MAT4_TRANS(renderer->model_matrix, position);
     glUniformMatrix4fv(renderer->model_uni_loc, 1, GL_FALSE, new_model.coeffs);

     glDrawElements(GL_TRIANGLES, r_obj->nb_elements,
                    GL_UNSIGNED_INT, (GLvoid*) 0);
    if (block->VAO_id != DEFAULT_VAO_ID) // Rebind the default VAO if needed
        glBindVertexArray(renderer->buffer_objs[DEFAULT_VAO_ID].VAO);
}

void block_free_renderer(blockRenderer* renderer) {
    block_free_all_textures(&renderer->block_list);
    free(renderer->block_list.blocks);
    for (uint8_t i = 0; i < renderer->nb_buffers; ++i) {
        renderBufferObject* buffer_obj = renderer->buffer_objs + i;
        glDeleteBuffers(1, &buffer_obj->VBO);
        glDeleteBuffers(1, &buffer_obj->EBO);
        glDeleteVertexArrays(1, &buffer_obj->VAO);
    }
    free(renderer->buffer_objs);
    glDeleteProgram(renderer->shader_program);
}

void block_load_texture(char* buffer, const char* prefix, const char* ext,
                            const char* suffix, GLuint* texture_id) {
    snprintf(buffer, 2 * MAX_BLOCK_STRLEN, "%s%s%s.png",
             prefix, ext, suffix);
    glGenTextures(1, texture_id);
    bind_load_texture(texture_id, buffer);
}

void block_try_load_texture(char* buffer, const char* prefix,
                                const char* ext, const char* suffix,
                                GLuint* tex_id, GLuint* def_tex_id) {
    snprintf(buffer, 2 * MAX_BLOCK_STRLEN, "%s%s%s.png",
             prefix, ext, suffix);
    if (!texture_exists(buffer)) {
        // If the texture cannot be loaded, set *tex_id to the default value
        *tex_id = *def_tex_id;
    } else {
        glGenTextures(1, tex_id);
        bind_load_texture(tex_id, buffer);
    }
}

void block_tex_load_from_render_type(char* buffer, const char* prefix,
                                         const char* suffix,
                                         renderType render_type,
                                         GLuint* tex_id) {
    if (render_type & RT_TOP)
        block_load_texture(buffer, prefix, "_top", suffix, tex_id++);
    if (render_type & RT_BOTTOM)
        block_load_texture(buffer, prefix, "_bottom", suffix, tex_id++);
    if (render_type & RT_FRONT)
        block_load_texture(buffer, prefix, "_front", suffix, tex_id++);
    if (render_type & RT_BACK)
        block_load_texture(buffer, prefix, "_back", suffix, tex_id++);
    if (render_type & RT_LEFT)
        block_load_texture(buffer, prefix, "_left", suffix, tex_id++);
    if (render_type & RT_RIGHT)
        block_load_texture(buffer, prefix, "_right", suffix, tex_id++);
    if (render_type & RT_SIDE)
        block_load_texture(buffer, prefix, "_side", suffix, tex_id++);
    if (render_type & RT_DEFAULT)
        block_load_texture(buffer, prefix, "", suffix, tex_id++);
}

void block_load_texture_BS_modificator(blockData* block) {
    char buffer[MAX_BLOCK_STRLEN * 2];
    char suffix[MAX_BLOCK_STRLEN];
    if (!block->bs_name_parser)
        errx(1, "block_load_texture_BS_modificator: No parser found...");
    blockStateValue bs_value = 0;
    while (block->bs_name_parser(bs_value, suffix) != NULL) {
        GLuint* tex_id = &block->textures[bs_value * block->nb_tex_draw];
        GLuint* def_tex_id = block->textures;
        if (block->render_type & RT_TOP)
            block_try_load_texture(buffer, block->name, "_top",
                                        suffix, tex_id++, def_tex_id++);
        if (block->render_type & RT_BOTTOM)
            block_try_load_texture(buffer, block->name, "_bottom",
                                        suffix, tex_id++, def_tex_id++);
        if (block->render_type & RT_FRONT)
            block_try_load_texture(buffer, block->name, "_front",
                                        suffix, tex_id++, def_tex_id++);
        if (block->render_type & RT_BACK)
            block_try_load_texture(buffer, block->name, "_back",
                                        suffix, tex_id++, def_tex_id++);
        if (block->render_type & RT_LEFT)
            block_try_load_texture(buffer, block->name, "_left",
                                        suffix, tex_id++, def_tex_id++);
        if (block->render_type & RT_RIGHT)
            block_try_load_texture(buffer, block->name, "_right",
                                        suffix, tex_id++, def_tex_id++);
        if (block->render_type & RT_SIDE)
            block_try_load_texture(buffer, block->name, "_side",
                                        suffix, tex_id++, def_tex_id++);
        if (block->render_type & RT_DEFAULT)
            block_try_load_texture(buffer, block->name, "",
                                        suffix, tex_id++, def_tex_id++);
        ++bs_value;
    }
}

void block_load_texture_BS_creator(blockData* block) {
    char buffer[MAX_BLOCK_STRLEN * 2];
    char prefix[MAX_BLOCK_STRLEN];
    blockStateKey bsk_used = CUB_BSK_TEX(block);
    if (!block->bs_name_parser)
        errx(1, "block_load_texture_BS_creator: No parser found...");
    char* name = BS_is_total_creator(bsk_used) ? NULL : block->name;
    char* start;
    blockStateValue bs_value = 0;
    while ( (start = block->bs_name_parser(bs_value, prefix)) != NULL ) {
        GLuint* tex_id = &block->textures[bs_value * block->nb_tex_draw];
        if (name) {
            *start = '_';
            strcpy(start + 1, name);
        }
        printf("prefix: %s\n", prefix);
        block_tex_load_from_render_type(buffer, prefix, "",
                                            block->render_type,
                                            tex_id);
        ++bs_value;
    }
}

void block_load_texture_pack(blockList* block_list) {
    printf("block_load_texture_pack: Starting to load texture pack...\n");
    char buffer[MAX_BLOCK_STRLEN * 2];
    for (size_t i = 0; i < block_list->nb_blocks; ++i) {
        blockData* current = &block_list->blocks[i];
        if (current->block_info.has_bs_tex) {
            if (current->block_info.is_bs_creator) // BS texture creator
                block_load_texture_BS_creator(current);
            else // BS texture modificator
                block_load_texture_BS_modificator(current);
        } else {
            // No blockstates which add additionnal textures
            block_tex_load_from_render_type(buffer, current->name, "",
                                                current->render_type,
                                                current->textures);
        }
    }
    printf("block_load_texture_pack: texture pack loaded!\n");
}

void block_free_all_textures(blockList* block_list) {
    printf("block_free_all_textures: Starting to free textures...\n");
    for (size_t i = 0; i < block_list->nb_blocks; ++i) {
        blockData* current = &block_list->blocks[i];
        uint16_t nb_modes = current->block_info.has_bs_tex
                            ? BS_TEX_get_nb_modes(CUB_BSK_TEX(current))
                            : 1;
        glDeleteTextures(current->nb_tex_draw * nb_modes, current->textures);
        free(current->textures);
    }
    printf("block_free_all_textures: Textures freed!\n");
}
