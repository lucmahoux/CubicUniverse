#include "utils/open_gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "utils/stb_image.h"

GLchar* load_texture(GLint* width, GLint* height,
                                GLint* nb_channels, const char* img_name) {
    char* fpath = utils_strconcat(TEXTURES_PATH, img_name, NULL);
    uint8_t* data = stbi_load(fpath, width, height, nb_channels, 0);
    if (!data)
        errx(1, "load_img: failed to load: %s", fpath);
    free(fpath);
    return (GLchar*)data;
}

bool texture_exists(const char* tex_name) {
    char* fpath = utils_strconcat(TEXTURES_PATH, tex_name, NULL);
    bool result = utils_file_exists(fpath);
    free(fpath);
    return result;
}

void bind_load_texture(GLuint* texture_id, const char* name) {
    glBindTexture(GL_TEXTURE_2D, *texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int width, height, nb_channels;
    GLchar* data = load_texture(&width, &height,
                                             &nb_channels, name);
    GLenum format = nb_channels == 3 ? GL_RGB :
                    nb_channels == 4 ? GL_RGBA : GL_RED;
    if (format == GL_RED)
        errx(1, "bind_load_texture: Wrong number of channels!"
                " Must be either 3 or 4: %s!", name);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}
