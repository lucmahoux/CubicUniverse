#ifndef CUB_GAME_GAME_H
#define CUB_GAME_GAME_H

#include "render/block.h"
#include "render/camera.h"
#include "render/chunk.h"
#include "render/chunk_loader.h"
#include "render/skybox.h"
#include "utils/open_gl.h"
#include "utils/utils.h"
#include "utils/window.h"

typedef struct cubGame {
    GLFWwindow* window;
    blockRenderer block_renderer;
    cubSkyboxRenderer skybox_renderer;
    camera camera;
    cubChunk chunk_test;
} cubGame;

void cub_game_init(cubGame* game, int width, int height);

void cub_game_clear_screen_handler(cubGame* game);

void cub_game_input_handler(cubGame* game);

void cub_game_renderer_handler(cubGame* game);

void cub_game_start(cubGame* game);

void cub_game_stop(cubGame* game);

#endif
