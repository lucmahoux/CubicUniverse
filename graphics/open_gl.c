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
