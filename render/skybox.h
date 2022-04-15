#ifndef CUB_RENDER_SKYBOX_H
#define CUB_RENDER_SKYBOX_H

#include "utils/utils.h"
#include "render/camera.h"
#include "render/open_gl.h"
#include "render/shader.h"
#include "utils/stb_image.h"

typedef struct cubSkyboxRenderer {
    RenderBufferObject RBO;
    GLuint shader_program;
    GLuint cubemapTexture;
    GLint VP_matrix_loc;
} SkyboxRenderer;

GLuint skybox_load_cubemap();

void skybox_setup_renderer(SkyboxRenderer* renderer);

void skybox_render(SkyboxRenderer* renderer, Camera* camera);

void skybox_free_renderer(SkyboxRenderer* renderer);

#endif
