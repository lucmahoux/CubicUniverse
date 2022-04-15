#include "block/block.h"

// ----------------------------------------------------------------------------

/* Loads the texture formed by the concatenation of (prefix, ext, suffix)
 * and add it to the texture atlas - the value *texture_quad is set
 * to the texture insertion position in the atlas. */
void block_load_texture(TextureAtlas* atlas, char* buffer,
                        const char* prefix, const char* ext,
                        const char* suffix, TexAtlasQuad* texture_quad);

/* Does exactly the same thing as cub_block_load_texture() but if the
 * texture file does NOT EXIST, then *tex_quad = *tex_quad_default. */
void block_try_load_texture(TextureAtlas* atlas, char* buffer,
                            const char* prefix, const char* ext,
                            const char* suffix,
                            TexAtlasQuad* tex_quad,
                            TexAtlasQuad* tex_quad_default);

/* Goes through all the possible render_type and load the textures
 * accordingly via calls to cub_block_load_texture() */
void block_tex_load_from_render_type(TextureAtlas* atlas, char* buffer,
                                     const char* prefix, const char* suffix,
                                     RenderType render_type,
                                     TexAtlasQuad* tex_quad);

/* Loads the textures of a block which has a blockstate texturer "Modificator".
 * It means that the BS modifies the appearance of the block without changing
 * its nature (e.g. furnace off/on) */
void block_load_texture_BS_modificator(TextureAtlas* atlas, BlockData* block);

/* Loads the textures of a block which has a blockstate texturer "Creator".
 * It means that the BS modifies the NATURE of a block
 * (e.g. acacia_planks VS dark_oak_planks) */
void block_load_texture_BS_creator(TextureAtlas* atlas, BlockData* block);

void block_get_uniforms_location_custom(CustomBlockRenderer* custom);

void block_get_uniforms_location_cubic(CubicBlockRenderer* cubic);

void block_cubic_setup_buffer_object(CubicBlockRenderer* cubic);

void block_custom_shader_setup_rotations_matrices(CustomBlockRenderer* custom);

mat4 get_translated_rotation_matrix(vec3 T1, float angle, vec3 axis, vec3 T2);

/* Returns the texture quad that should be applied to the face identified by
 * 'face_ID' of the blockstate 'bs' (rotations handled) - CUBIC BLOCKS ONLY */
TexAtlasQuad block_get_texture_cubic(BlockState* BS, FaceID face_ID);

/* Used to retrieve the packed shader data (with bit fields) required to draw
 * CUBIC block faces */
void block_shader_cubic_set_packed_data(BlockState* BS, FaceID face_ID,
                                        SubChunkCoords sc_coords,
                                        GLuint* packed_data);

/* Called when the block to render is NOT CUBIC.
 * Textures quad data is assigned by following the order of textures:
 * TOP(0) -> BOTTOM(1) -> FRONT(2) -> BACK(3) -> LEFT(4) -> RIGHT(5) */
void block_shader_custom_set_texture_data(BlockState* BS, GLuint* tex_data);

/* Used to retrieve the packed shader data (with bit fields) required to draw
 * the CUSTOM block */
void block_shader_custom_set_packed_data(BlockState* BS,
                                         SubChunkCoords sc_coords,
                                         GLuint* packed_data);
// ----------------------------------------------------------------------------

void block_get_uniforms_location_custom(CustomBlockRenderer* custom) {
    const char fname[] = "block_get_uniforms_location_custom";
    struct cubCustomBlockShaderUniforms* uniforms = &custom->uniforms;
    try_get_uniform_location(custom->shader, &uniforms->texture_atlas,
                             "texture_atlas", fname);
    try_get_uniform_location(custom->shader, &uniforms->texture_atlas_dim,
                             "texture_atlas_dim", fname);
    try_get_uniform_location(custom->shader, &uniforms->VP_matrix,
                             "VP_matrix", fname);
    try_get_uniform_location(custom->shader, &uniforms->texture_data,
                             "texture_data", fname);
    try_get_uniform_location(custom->shader, &uniforms->packed_data,
                             "packed_data", fname);
    try_get_uniform_location(custom->shader, &uniforms->subchunk_pos,
                             "subchunk_pos", fname);
    try_get_uniform_location(custom->shader, &uniforms->primary_rotations,
                             "primary_rotations", fname);
    try_get_uniform_location(custom->shader, &uniforms->secondary_rotations,
                             "secondary_rotations", fname);
}

void block_get_uniforms_location_cubic(CubicBlockRenderer* cubic) {
    const char fname[] = "block_get_uniforms_location_cubic";
    struct cubCubicBlockShaderUniforms* uniforms = &cubic->uniforms;
    try_get_uniform_location(cubic->shader, &uniforms->texture_atlas,
                             "texture_atlas", fname);
    try_get_uniform_location(cubic->shader, &uniforms->texture_atlas_dim,
                             "texture_atlas_dim", fname);
    try_get_uniform_location(cubic->shader, &uniforms->texel_padding,
                             "texel_padding", fname);
    try_get_uniform_location(cubic->shader, &uniforms->VP_matrix,
                             "VP_matrix", fname);
    try_get_uniform_location(cubic->shader, &uniforms->subchunk_pos,
                             "subchunk_pos", fname);
}

void block_cubic_setup_buffer_object(CubicBlockRenderer* cubic) {
    RenderBufferObject* buffer_obj = &cubic->RBO;
    setup_render_buffer_object(buffer_obj);
    glGenBuffers(1, &buffer_obj->VBO);
    glGenVertexArrays(1, &buffer_obj->VAO);
    glBindVertexArray(buffer_obj->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_obj->VBO);
    glBufferData(GL_ARRAY_BUFFER, SHADER_CUBIC_PACKED_DATA_SIZE
                 * CUBIC_VBO_BUFFER_SIZE * sizeof(GLuint),
                 NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(0, 2, GL_UNSIGNED_INT, SHADER_CUBIC_PACKED_DATA_SIZE
                           * sizeof(GLuint), (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

mat4 get_translated_rotation_matrix(vec3 T1, float angle, vec3 axis, vec3 T2) {
    mat4 result = MAT4(1.0f);
    result.coeffs[12] = T2.coords[0];
    result.coeffs[13] = T2.coords[1];
    result.coeffs[14] = T2.coords[2];
    result = MAT4_ROT(result, angle, axis);
    result = MAT4_TRANS(result, T1);
    return result;
}

void block_custom_shader_setup_rotations_matrices(CustomBlockRenderer* custom) {
    glUseProgram(custom->shader);
    // Primary rotations
    GLfloat primary_coeffs[16 * 3];
    vec3 T1 = VEC3(-0.5f, 0.0f, -0.5f);
    vec3 T2 = VEC3(0.5f, 0.0f, 0.5f);
    vec3 axis = VEC3(0.0f, 1.0f, 0.0f);
    // -> MID 90°
    mat4 primary_rotation =
        get_translated_rotation_matrix(T1, M_PI / 2.0f, axis, T2);
    memcpy(primary_coeffs, primary_rotation.coeffs, 16 * sizeof(float));
    // -> MID 180°
    primary_rotation = get_translated_rotation_matrix(T1, M_PI, axis, T2);
    memcpy(primary_coeffs + 16, primary_rotation.coeffs, 16 * sizeof(float));
    // -> MID 270°
    primary_rotation =
        get_translated_rotation_matrix(T1, -M_PI / 2.0f, axis, T2);
    memcpy(primary_coeffs + 32, primary_rotation.coeffs, 16 * sizeof(float));

    // Secondary rotations
    GLfloat sec_coeffs[16 * 8];
    mat4 identity = MAT4(1.0f);
    axis = VEC3(0.0f, 0.0f, 1.0f);
    // -> Z LEFT 90°
    mat4 secondary_rotation = MAT4_ROT(identity, M_PI / 2.0f, axis);
    memcpy(sec_coeffs, secondary_rotation.coeffs, 16 * sizeof(float));
    // -> Z LEFT 270°
    secondary_rotation = MAT4_ROT(identity, -M_PI / 2.0f, axis);
    memcpy(sec_coeffs + 16, secondary_rotation.coeffs, 16 * sizeof(float));
    // -> Z RIGHT 90°
    T1 = VEC3(-1.0f, 0.0f, 0.0f);
    T2 = VEC3(1.0f, 0.0f, 0.0f);
    secondary_rotation =
        get_translated_rotation_matrix(T1, -M_PI / 2.0f, axis, T2);
    memcpy(sec_coeffs + 16 * 2, secondary_rotation.coeffs, 16 * sizeof(float));
    // -> Z RIGHT 270°
    secondary_rotation =
        get_translated_rotation_matrix(T1, M_PI / 2.0f, axis, T2);
    memcpy(sec_coeffs + 16 * 3, secondary_rotation.coeffs, 16 * sizeof(float));
    // -> X BACK 90°
    axis = VEC3(1.0f, 0.0f, 0.0f);
    secondary_rotation = MAT4_ROT(identity, M_PI / 2.0f, axis);
    memcpy(sec_coeffs + 16 * 4, secondary_rotation.coeffs, 16 * sizeof(float));
    // -> X BACK 270°
    secondary_rotation = MAT4_ROT(identity, -M_PI / 2.0f,  axis);
    memcpy(sec_coeffs + 16 * 5, secondary_rotation.coeffs, 16 * sizeof(float));
    // -> X FRONT 90°
    T1 = VEC3(0.0f, 0.0f, -1.0f);
    T2 = VEC3(0.0f, 0.0f, 1.0f);
    secondary_rotation =
        get_translated_rotation_matrix(T1, -M_PI / 2.0f, axis, T2);
    memcpy(sec_coeffs + 16 * 6, secondary_rotation.coeffs, 16 * sizeof(float));
    // -> X FRONT 270°
    secondary_rotation =
        get_translated_rotation_matrix(T1, M_PI / 2.0f, axis, T2);
    memcpy(sec_coeffs + 16 * 7, secondary_rotation.coeffs, 16 * sizeof(float));

    // Uniforms update
    glUniformMatrix4fv(custom->uniforms.primary_rotations, 3, GL_FALSE,
                       primary_coeffs);
    glUniformMatrix4fv(custom->uniforms.secondary_rotations, 8, GL_FALSE,
                       sec_coeffs);
}

void block_setup_renderer_and_block_list(BlockRenderer* block_renderer,
                                         BlockList* block_list) {
    printf("block_setup_renderer: Starting renderer initialisation...\n");

    // Load the shader programs
    block_renderer->cubic.shader = build_shader("block_cubic");
    block_renderer->custom.shader = build_shader("block_custom");

    // Set the uniforms locations of the shaders
    block_get_uniforms_location_cubic(&block_renderer->cubic);
    block_get_uniforms_location_custom(&block_renderer->custom);

    // Load the block list & the rendering buffers
    setup_texture_atlas(&block_renderer->texture_atlas, CUB_TEX_ATLAS_WIDTH);
    BLloader_load(block_list, &block_renderer->custom.RBO_chunks,
                  &block_renderer->custom.RBO);
    block_cubic_setup_buffer_object(&block_renderer->cubic);
    block_renderer->cubic.cache_offset = 0;
    block_load_texture_pack(block_renderer, block_list);
    block_custom_shader_setup_rotations_matrices(&block_renderer->custom);

    // Enable depth testing, otherwise weird rendering of cubes
    glEnable(GL_DEPTH_TEST);
    // Accept the fragment if closer to the camera than the former one
    glDepthFunc(GL_LESS);

    printf("block_setup_renderer: Renderer initialised!\n");
}

TexAtlasQuad block_get_texture_cubic(BlockState* BS, FaceID face_ID) {
    // TODO: Apply rotational blockstates (= change face_ID)
    uint8_t base_offset = (BS->block->flags.has_bs_tex
                      ? BS->states[BS_TEX_ID]
                      : 0) * BS->block->nb_tex_draw;

    uint8_t face_offset = base_offset;
    for (FaceID face = FID_START; face < face_ID; ++face) {
        RenderType render_type_face = 1 << face;
        if (BS->block->render_type & render_type_face)
            ++face_offset;
    }

    switch (face_ID) {
        case FID_TOP:
            return BS->block->render_type & RT_TOP
                ? BS->block->textures[face_offset] // = top texture
                : BS->block->render_type & RT_BOTTOM
                ? BS->block->textures[base_offset] // = bottom texture
                : BS->block->textures[base_offset + BS->block->nb_tex_draw - 1];
        case FID_BOTTOM:
            return BS->block->render_type & RT_BOTTOM
                ? BS->block->textures[face_offset] // = bottom texture
                : BS->block->render_type & RT_TOP
                ? BS->block->textures[base_offset] // = top texture
                : BS->block->textures[base_offset + BS->block->nb_tex_draw - 1];
        default: ;
            RenderType render_type_face = 1 << face_ID;
            return BS->block->render_type & render_type_face
                ? BS->block->textures[face_offset] // = front|back|left|right
                : BS->block->render_type & RT_SIDE
                ? BS->block->textures[base_offset + BS->block->nb_tex_draw -
                    (BS->block->render_type & RT_DEFAULT ? 2 : 1)] // = side
                : BS->block->textures[base_offset + BS->block->nb_tex_draw - 1];
    }
}

void block_shader_custom_set_texture_data(BlockState* BS, GLuint* tex_data) {
    uint8_t offset = (BS->block->flags.has_bs_tex
                      ? BS->states[BS_TEX_ID]
                      : 0) * BS->block->nb_tex_draw;
    memset(tex_data, 0, SHADER_CUSTOM_TEXTURE_DATA_SIZE * sizeof(GLuint));
    for (uint8_t i = 0; i < BS->block->nb_tex_draw; ++i) {
        tex_data[i / 2] += BS->block->textures[offset + i].val.offsetY
                            << (16 * (i % 2));
        tex_data[3] += BS->block->textures[offset + i].val.offsetX << (5 * i);
        tex_data[4] +=
            BS->block->textures[offset + i].val.texture_type << (3 * i);
    }
}

void block_shader_custom_set_packed_data(cub_unused BlockState* BS,
                                         SubChunkCoords sc_coords,
                                         GLuint* packed_data) {
    // TODO: rotations & all
    packed_data[0] = (sc_coords.x << 28) + (sc_coords.y << 24)
                   + (sc_coords.z << 20);
}

void block_shader_cubic_set_packed_data(BlockState* BS, FaceID face_ID,
                                        SubChunkCoords sc_coords,
                                        GLuint* packed_data) {
    TexAtlasQuad tex_quad = block_get_texture_cubic(BS, face_ID);
    memset(packed_data, 0, SHADER_CUBIC_PACKED_DATA_SIZE * sizeof(GLuint));
    packed_data[0] = (face_ID << 29) + (tex_quad.val.offsetX << 24)
                    + (tex_quad.val.offsetY << 8)
                    +(tex_quad.val.texture_type << 5)
                    + (sc_coords.x << 1);
    packed_data[1] = (sc_coords.y << 28) + (sc_coords.z << 24);
}

bool block_cubic_add_to_cache(CubicBlockRenderer* cubic, BlockState* BS,
                              FaceID face, SubChunkCoords position) {
    bool emptied = false;
    if (cubic->cache_offset == CUBIC_VBO_BUFFER_SIZE) {
        glUseProgram(cubic->shader);
        glBindBuffer(GL_ARRAY_BUFFER, cubic->RBO.VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, CUBIC_VBO_BUFFER_SIZE *
                        SHADER_CUBIC_TOTAL_UINTS * sizeof(GLuint),
                        cubic->cache);
        glBindVertexArray(cubic->RBO.VAO);
        glDrawArrays(GL_POINTS, 0, CUBIC_VBO_BUFFER_SIZE);
        cubic->cache_offset = 0;
        emptied = true;
    }
    GLuint* ptr = cubic->cache + cubic->cache_offset * SHADER_CUBIC_TOTAL_UINTS;
    block_shader_cubic_set_packed_data(BS, face, position, ptr);
    ++cubic->cache_offset;
    return emptied;
}

void block_cubic_flush_cache(CubicBlockRenderer* cubic) {
    if (cubic->cache_offset == 0)
        return;
    glUseProgram(cubic->shader);
    glBindBuffer(GL_ARRAY_BUFFER, cubic->RBO.VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, cubic->cache_offset *
                    SHADER_CUBIC_TOTAL_UINTS * sizeof(GLuint),
                    cubic->cache);
    glBindVertexArray(cubic->RBO.VAO);
    glDrawArrays(GL_POINTS, 0, cubic->cache_offset);
    cubic->cache_offset = 0;
}

void block_custom_render(CustomBlockRenderer* custom, BlockState* BS,
                         SubChunkCoords position) {
    struct cubCustomBlockInfoRBO* info_RBO = custom->RBO_chunks +
                                             BS->block->info_RBO_id;
    GLuint texture_data[SHADER_CUSTOM_TEXTURE_DATA_SIZE];
    block_shader_custom_set_texture_data(BS, texture_data);
    glUniform1uiv(custom->uniforms.texture_data,
                  SHADER_CUSTOM_TEXTURE_DATA_SIZE,
                  texture_data);
    GLuint packed_data[SHADER_CUSTOM_PACKED_DATA_SIZE];
    block_shader_custom_set_packed_data(BS, position, packed_data);
    glUniform1uiv(custom->uniforms.packed_data,
                 SHADER_CUSTOM_PACKED_DATA_SIZE,
                 packed_data);
    glBindVertexArray(custom->RBO.VAO);
    glDrawElements(GL_TRIANGLES, info_RBO->nb_elements, GL_UNSIGNED_INT,
                   (const GLvoid*)(size_t)info_RBO->element_offset);
}

void block_free_renderer(BlockRenderer* renderer) {
    free_render_buffer_object(&renderer->custom.RBO);
    free(renderer->custom.RBO_chunks);
    free_render_buffer_object(&renderer->cubic.RBO);
    glDeleteProgram(renderer->cubic.shader);
    glDeleteProgram(renderer->custom.shader);
    glDeleteTextures(1, &renderer->texture_atlas.atlas_texture_id);
}

void block_load_texture(TextureAtlas* atlas, char* buffer,
                        const char* prefix, const char* ext,
                        const char* suffix, TexAtlasQuad* texture_quad) {
    snprintf(buffer, 2 * MAX_BLOCK_STRLEN, "%s%s%s.png", prefix, ext, suffix);
    texture_atlas_add(atlas, buffer, texture_quad);
}

void block_try_load_texture(TextureAtlas* atlas,
                            char* buffer, const char* prefix,
                            const char* ext, const char* suffix,
                            TexAtlasQuad* tex_quad,
                            TexAtlasQuad* tex_quad_default) {
    snprintf(buffer, 2 * MAX_BLOCK_STRLEN, "%s%s%s.png", prefix, ext, suffix);
    if (!texture_exists(buffer)) {
        // If the texture cannot be loaded, set *tex_id to the default value
        *tex_quad = *tex_quad_default;
    } else texture_atlas_add(atlas, buffer, tex_quad);
}

void block_tex_load_from_render_type(TextureAtlas* atlas,
                                     char* buffer, const char* prefix,
                                     const char* suffix,
                                     RenderType render_type,
                                     TexAtlasQuad* tex_quad) {
    if (render_type & RT_TOP)
        block_load_texture(atlas, buffer, prefix, "_top", suffix, tex_quad++);
    if (render_type & RT_BOTTOM)
        block_load_texture(atlas, buffer, prefix, "_bottom", suffix,tex_quad++);
    if (render_type & RT_FRONT)
        block_load_texture(atlas, buffer, prefix, "_front", suffix, tex_quad++);
    if (render_type & RT_BACK)
        block_load_texture(atlas, buffer, prefix, "_back", suffix, tex_quad++);
    if (render_type & RT_LEFT)
        block_load_texture(atlas, buffer, prefix, "_left", suffix, tex_quad++);
    if (render_type & RT_RIGHT)
        block_load_texture(atlas, buffer, prefix, "_right", suffix, tex_quad++);
    if (render_type & RT_SIDE)
        block_load_texture(atlas, buffer, prefix, "_side", suffix, tex_quad++);
    if (render_type & RT_DEFAULT)
        block_load_texture(atlas, buffer, prefix, "", suffix, tex_quad++);
}

void block_load_texture_BS_modificator(TextureAtlas* atlas, BlockData* block) {
    char buffer[MAX_BLOCK_STRLEN * 2];
    char suffix[MAX_BLOCK_STRLEN];
    if (!block->bs_name_parser)
        errx(1, "block_load_texture_BS_modificator: No parser found...");
    BlockStateValue bs_value = 0;
    while (block->bs_name_parser(bs_value, suffix) != NULL) {
        TexAtlasQuad* tex_quad =
            block->textures + bs_value * block->nb_tex_draw;
        TexAtlasQuad* tex_quad_default = block->textures;
        if (block->render_type & RT_TOP)
            block_try_load_texture(atlas, buffer, block->name, "_top",
                                   suffix, tex_quad++, tex_quad_default++);
        if (block->render_type & RT_BOTTOM)
            block_try_load_texture(atlas, buffer, block->name, "_bottom",
                                   suffix, tex_quad++, tex_quad_default++);
        if (block->render_type & RT_FRONT)
            block_try_load_texture(atlas, buffer, block->name, "_front",
                                   suffix, tex_quad++, tex_quad_default++);
        if (block->render_type & RT_BACK)
            block_try_load_texture(atlas, buffer, block->name, "_back",
                                   suffix, tex_quad++, tex_quad_default++);
        if (block->render_type & RT_LEFT)
            block_try_load_texture(atlas, buffer, block->name, "_left",
                                   suffix, tex_quad++, tex_quad_default++);
        if (block->render_type & RT_RIGHT)
            block_try_load_texture(atlas, buffer, block->name, "_right",
                                   suffix, tex_quad++, tex_quad_default++);
        if (block->render_type & RT_SIDE)
            block_try_load_texture(atlas, buffer, block->name, "_side",
                                   suffix, tex_quad++, tex_quad_default++);
        if (block->render_type & RT_DEFAULT)
            block_try_load_texture(atlas, buffer, block->name, "",
                                   suffix, tex_quad++, tex_quad_default++);
        ++bs_value;
    }
}

void block_load_texture_BS_creator(TextureAtlas* atlas, BlockData* block) {
    char buffer[MAX_BLOCK_STRLEN * 2];
    char prefix[MAX_BLOCK_STRLEN];
    BlockStateKey bsk_used = CUB_BSK_TEX(block);
    if (!block->bs_name_parser)
        errx(1, "block_load_texture_BS_creator: No parser found...");
    char* name = BS_is_total_creator(bsk_used) ? NULL : block->name;
    char* start;
    BlockStateValue bs_value = 0;
    while ( (start = block->bs_name_parser(bs_value, prefix)) != NULL ) {
        TexAtlasQuad* tex_quad =
            block->textures + bs_value * block->nb_tex_draw;
        if (name) {
            *start = '_';
            strcpy(start + 1, name);
        }
        block_tex_load_from_render_type(atlas, buffer, prefix, "",
                                        block->render_type, tex_quad);
        ++bs_value;
    }
}

void block_load_texture_pack(BlockRenderer* block_renderer,
                             BlockList* block_list) {
    printf("block_load_texture_pack: Starting to load texture pack...\n");
    TextureAtlas* atlas = &block_renderer->texture_atlas;
    if (atlas->atlas_texture_id) {
        glDeleteTextures(1, &atlas->atlas_texture_id);
        setup_texture_atlas(atlas, CUB_TEX_ATLAS_WIDTH);
    }
    for (size_t i = 0; i < block_list->nb_blocks; ++i) {
        BlockData* current = block_list->blocks + i;
        if (current->flags.has_bs_tex) {
            if (current->flags.is_bs_creator) // BS texture creator
                block_load_texture_BS_creator(atlas, current);
            else // BS texture modificator
                block_load_texture_BS_modificator(atlas, current);
        } else {
            // No blockstates which add additionnal textures
            char buffer[MAX_BLOCK_STRLEN * 2];
            block_tex_load_from_render_type(atlas, buffer, current->name, "",
                                            current->render_type,
                                            current->textures);
        }
    }

    // Bind & Load the texture atlas
    texture_atlas_bind_load_GPU(atlas);

    // Set the texture atlas uniform
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas->atlas_texture_id);

    glUseProgram(block_renderer->custom.shader);
    struct cubCustomBlockShaderUniforms* custom_uniforms =
        &block_renderer->custom.uniforms;
    texture_atlas_set_uniforms(atlas, custom_uniforms->texture_atlas,
                               custom_uniforms->texture_atlas_dim, -1);

    glUseProgram(block_renderer->cubic.shader);
    struct cubCubicBlockShaderUniforms* cubic_uniforms =
        &block_renderer->cubic.uniforms;
    texture_atlas_set_uniforms(atlas, cubic_uniforms->texture_atlas,
                               cubic_uniforms->texture_atlas_dim,
                               cubic_uniforms->texel_padding);
    printf("block_load_texture_pack: texture pack loaded!\n");
}
