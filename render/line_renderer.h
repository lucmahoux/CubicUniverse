#ifndef CUB_RENDER_LINE_RENDERER_H
#define CUB_RENDER_LINE_RENDERER_H

#include "utils/utils.h"
#include "render/open_gl.h"
#include "render/shader.h"

typedef struct cubLineRenderer {
    RenderBufferObject RBO;
    GLuint shader_program;
    GLint VP_matrix_loc;
} LineRenderer;

void line_renderer_setup(LineRenderer* renderer);

void line_render(LineRenderer* renderer, vec3 start, vec3 end, vec3 color);

void line_renderer_free(LineRenderer* renderer);

#endif
