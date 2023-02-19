#ifndef CUB_RENDER_SKYBOX_H
#define CUB_RENDER_SKYBOX_H

#include "utils.h"
#include "camera.h"
#include "open_gl.h"
#include "shader.h"
#include "stb_image.h"

typedef struct cubSkyboxRenderer {
    RenderBufferObject RBO;
    GLuint shader_program;
    GLuint cubemapTexture;
    GLint VP_matrix_loc;
} SkyboxRenderer;

void skybox_setup_renderer(SkyboxRenderer* renderer);

void skybox_render(SkyboxRenderer* renderer, Camera* camera);

void skybox_free_renderer(SkyboxRenderer* renderer);

#endif
