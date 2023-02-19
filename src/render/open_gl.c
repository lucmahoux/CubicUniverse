#include "open_gl.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

GLchar* load_texture(GLint* width, GLint* height, GLint* nb_channels,
                     const char* img_name) {
    char* path = utils_strconcat(TEXTURES_PATH, img_name, NULL);
    uint8_t* data = stbi_load(path, width, height, nb_channels, 0);
    if (!data)
        errx(1, "load_img: failed to load: %s", path);
    free(path);
    return (GLchar*)data;
}

bool texture_exists(const char* tex_name) {
    char* path = utils_strconcat(TEXTURES_PATH, tex_name, NULL);
    bool result = utils_file_exists(path);
    free(path);
    return result;
}

void try_get_uniform_location(GLuint shader_program, GLint* location,
                              const char* uniform_name, const char* fname) {
    *location = glGetUniformLocation(shader_program, uniform_name);
    if (*location == -1)
        errx(EXIT_FAILURE, "%s: Error - could not find the uniform '%s'",
             fname, uniform_name);
}

void setup_render_buffer_object(RenderBufferObject* buffer_obj) {
    buffer_obj->VBO = 0;
    buffer_obj->EBO = 0;
    buffer_obj->VAO = 0;
    buffer_obj->nb_elements = 0;
}

void free_render_buffer_object(RenderBufferObject* buffer_obj) {
    glDeleteBuffers(1, &buffer_obj->VBO);
    glDeleteBuffers(1, &buffer_obj->EBO);
    glDeleteVertexArrays(1, &buffer_obj->VAO);
}
