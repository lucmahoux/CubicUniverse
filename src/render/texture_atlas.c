#include "texture_atlas.h"
#include "stb_image_write.h"

void setup_texture_atlas(TextureAtlas* atlas, size_t fixed_width) {
    memset(atlas, 0, sizeof(TextureAtlas));
    atlas->fixed_width = fixed_width;
    atlas->height = 0;
    atlas->last_16.val.texture_type = 0;
    atlas->last_32.val.texture_type = 1;
    atlas->last_64.val.texture_type = 2;
    atlas->last_128.val.texture_type = 3;
    atlas->last_256.val.texture_type = 4;
    atlas->last_512.val.texture_type = 5;
    printf("setup_texture_atlas: texture atlas initialised!\n");
}

#define ATLAS_REALLOC(DIM)\
    atlas->last_##DIM.val.has_been_added = true;\
    atlas->last_##DIM.val.offsetX = 0;\
    atlas->last_##DIM.val.offsetY = atlas->height >> 4;\
    atlas->height += DIM;\
    atlas->pixels = utils_realloc(atlas->pixels,\
                                  atlas->fixed_width * atlas->height * 4,\
                                  fname);\


#define TEXTURE_ADD_DIM(DIM)\
    if (!atlas->last_##DIM.val.has_been_added) {\
        ATLAS_REALLOC(DIM);\
    }\
    dest = atlas->pixels + (atlas->last_##DIM.val.offsetY * atlas->fixed_width\
            + atlas->last_##DIM.val.offsetX) * 16 * 4;\
    p = data;\
    for (size_t y = 0; y < DIM;\
         ++y, dest += atlas->fixed_width * 4, p += DIM * 4)\
        memcpy(dest, p, DIM * 4);\
    *texture = atlas->last_##DIM;\
    atlas->last_##DIM.val.offsetX += (DIM >> 4);\
    if (atlas->last_##DIM.val.offsetX == 0) {\
        ATLAS_REALLOC(DIM);\
    }

bool texture_atlas_add(TextureAtlas* atlas, const char* texture_name,
                       TexAtlasQuad* texture) {
    char* fpath = utils_strconcat(TEXTURES_PATH, texture_name, NULL);
    int width, height, nb_channels;
    uint8_t* data = stbi_load(fpath, &width, &height, &nb_channels, 4);
    if (!data)
        errx(EXIT_FAILURE, "texture_atlas_add: failed to load %s!", fpath);
    if (width != height) {
        fprintf(stderr, "texture_atlas_add: ILLEGAL - width != height!\n");
        return false;
    } else if (width != 16 && width != 32 && width != 64 && width != 128
                && width != 256 && width != 512) {
        fprintf(stderr, "texture_atlas_add: ILLEGAL - side must be either"
                " 16, 32, 64, 128, 256 or 512 pixels-length!\n");
        return false;
    }
    const char fname[] = "texture_atlas_add";
    uint8_t* dest;
    uint8_t* p;
    switch (width) {
        case 16:
            TEXTURE_ADD_DIM(16); break;
        case 32:
            TEXTURE_ADD_DIM(32); break;
        case 64:
            TEXTURE_ADD_DIM(64); break;
        case 128:
            TEXTURE_ADD_DIM(128); break;
        case 256:
            TEXTURE_ADD_DIM(256); break;
        default:
            TEXTURE_ADD_DIM(512); break;
    }
    stbi_image_free(data);
    free(fpath);
    return true;
}

void texture_atlas_bind_load_GPU(TextureAtlas* texture_atlas) {
    glGenTextures(1, &texture_atlas->atlas_texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_atlas->atlas_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_atlas->fixed_width,
                 texture_atlas->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 texture_atlas->pixels);
    stbi_write_png("../texture_atlas.png", (int)texture_atlas->fixed_width,
                   (int)texture_atlas->height, 4, texture_atlas->pixels,
                   (int)(4 * texture_atlas->fixed_width));
    free(texture_atlas->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_atlas_set_uniforms(TextureAtlas* texture_atlas,
                                GLint texture_atlas_sampler_loc,
                                GLint texture_atlas_dim_vec2_loc,
                                GLint texel_padding_vec2_loc) {
    if (texture_atlas_sampler_loc != -1)
        glUniform1i(texture_atlas_sampler_loc, 0);
    float atlas_width = (float)(texture_atlas->fixed_width >> 4);
    float atlas_height = (float)(texture_atlas->height >> 4);
    if (texture_atlas_dim_vec2_loc != -1)
        glUniform2f(texture_atlas_dim_vec2_loc, atlas_width, atlas_height);
    if (texel_padding_vec2_loc != -1) {
        glUniform2f(texel_padding_vec2_loc,
                    0.5f / 16.0f / atlas_width,
                    0.5f / 16.0f / atlas_height);
    }
}
