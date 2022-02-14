#ifndef CUB_RENDER_SKYBOX_H
#define CUB_RENDER_SKYBOX_H

#include "utils/utils.h"
#include "utils/open_gl.h"
#include "utils/shader.h"
#include "utils/stb_image.h"

typedef struct cubSkyboxRenderer {
    GLuint VAO;
    GLuint VBO, EBO;
    GLuint shader_program;
    GLuint cubemapTexture;
} cubSkyboxRenderer;

GLuint cub_skybox_load_cubemap();

void cub_skybox_setup_renderer(cubSkyboxRenderer* renderer);

void cub_skybox_render(cubSkyboxRenderer* renderer);

#endif
