#ifndef CUB_RENDER_AXES_H
#define CUB_RENDER_AXES_H

#include "utils/utils.h"
#include "render/camera.h"
#include "render/open_gl.h"
#include "render/shader.h"

typedef struct cubAxesRenderer {
    RenderBufferObject RBO;
    GLuint shader_program;
    GLint VP_matrix_loc;
} AxesRenderer;

void axes_setup_renderer(AxesRenderer* renderer);

void axes_render(AxesRenderer* renderer, Camera* camera);

void axes_free_renderer(AxesRenderer* renderer);

#endif
