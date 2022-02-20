#ifndef CUB_RENDER_SKYBOX_H
#define CUB_RENDER_SKYBOX_H

#include "utils/utils.h"
#include "utils/open_gl.h"
#include "utils/shader.h"
#include "utils/stb_image.h"

typedef struct skyboxRenderer {
    GLuint VAO;
    GLuint VBO;
    GLuint shader_program;
    GLuint cubemapTexture;
    GLuint projection_uni_loc;
    GLuint view_uni_loc;
} skyboxRenderer;

GLuint skybox_load_cubemap();

void skybox_setup_renderer(skyboxRenderer* renderer);

void skybox_free_renderer(skyboxRenderer* renderer);

#endif
