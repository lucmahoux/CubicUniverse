#ifndef CUB_UTILS_OPEN_GL_H
#define CUB_UTILS_OPEN_GL_H

#include "utils/utils.h"

typedef struct renderBufferObject {
    // Buffers identifiers
    GLuint VBO, EBO, VAO;
    // Number of elements to draw
    GLsizei nb_elements;
} renderBufferObject;

/* Load the texture from the texture folder - check for errors */
GLchar* load_texture(GLint* width, GLint* height,
                                    GLint* nb_channels, const char* img_name);

/* Returns TRUE if the given textureexists */
bool texture_exists(const char* tex_name);

/* Bind & load the texture from with its openGL id */
void bind_load_texture(GLuint* texture_id, const char* name);

#endif
