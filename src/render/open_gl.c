#include "open_gl.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

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
