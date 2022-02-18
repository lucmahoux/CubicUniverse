#ifndef CUB_UTILS_OPEN_GL_H
#define CUB_UTILS_OPEN_GL_H

#include "utils/utils.h"

typedef struct cubRenderBufferObject {
    // Buffers identifiers
    GLuint VBO, EBO, VAO;
    // Number of elements to draw
    GLsizei nb_elements;
} cubRenderBufferObject;

/* Load the texture from the texture folder - check for errors */
GLchar* cub_utils_load_texture(GLint* width, GLint* height,
                                    GLint* nb_channels, const char* img_name);

/* Returns TRUE if the given textureexists */
bool cub_utils_texture_exists(const char* tex_name);

/* Bind & load the texture from with its openGL id */
void cub_utils_bind_load_texture(GLuint* texture_id, const char* name);

#endif
