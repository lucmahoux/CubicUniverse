#include "graphics/open_gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "utils/stb_image.h"

GLchar* cub_graphics_load_texture(GLint* width, GLint* height,
                                GLint* nb_channels, const char* img_name) {
    char* fpath = cub_utils_strconcat(TEXTURES_PATH, img_name, NULL);
    uint8_t* data = stbi_load(fpath, width, height, nb_channels, 0);
    if (!data)
        errx(1, "cub_graphics_load_img: failed to load: %s", fpath);
    free(fpath);
    return (GLchar*)data;
}

void cub_graphics_bind_load_texture(GLuint* texture_id, const char* name) {
    glBindTexture(GL_TEXTURE_2D, *texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nb_channels;
    GLchar* data = cub_graphics_load_texture(&width, &height,
                                             &nb_channels, name);
    if (nb_channels != 4)
        errx(1, "cub_graphics_bind_load_texture: all textures are expected\
                to have an alpha channel (RGBA not RGB)!");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(data);
}
