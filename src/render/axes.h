#ifndef CUB_RENDER_AXES_H
#define CUB_RENDER_AXES_H

#include "utils.h"
#include "camera.h"
#include "open_gl.h"
#include "shader.h"

typedef struct cubAxesRenderer {
    RenderBufferObject RBO;
    GLuint shader_program;
    GLint VP_matrix_loc;
} AxesRenderer;

void axes_setup_renderer(AxesRenderer* renderer);

void axes_render(AxesRenderer* renderer, Camera* camera);

void axes_free_renderer(AxesRenderer* renderer);

#endif
