#ifndef CUB_OPEN_GL_TYPES_H
#define CUB_OPEN_GL_TYPES_H

#include "utils.h"

typedef struct cubRenderBufferObject {
    // Buffers identifiers
    GLuint VBO, EBO, VAO;
    // Number of elements to draw
    GLsizei nb_elements;
} RenderBufferObject;

#endif /* !CUB_OPEN_GL_TYPES_H */
