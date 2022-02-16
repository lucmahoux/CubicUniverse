#ifndef CUB_RENDER_SKYBOX_H
#define CUB_RENDER_SKYBOX_H

#include "utils/utils.h"
#include "utils/open_gl.h"
#include "utils/shader.h"
#include "utils/stb_image.h"

typedef struct cubSkyboxRenderer {
    GLuint VAO;
    GLuint VBO;
    GLuint shader_program;
    GLuint cubemapTexture;
    GLuint projection_uni_loc;
    GLuint view_uni_loc;
} cubSkyboxRenderer;

GLuint cub_skybox_load_cubemap();

void cub_skybox_setup_renderer(cubSkyboxRenderer* renderer);

void cub_skybox_free_renderer(cubSkyboxRenderer* renderer);

#endif
