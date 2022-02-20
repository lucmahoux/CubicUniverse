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

typedef struct game {
    GLFWwindow* window;
    blockRenderer block_renderer;
    skyboxRenderer skybox_renderer;
    camera camera;
    chunk chunk_test;
} game;

void game_init(game* game, int width, int height);

void game_clear_screen_handler(game* game);

void game_input_handler(game* game);

void game_renderer_handler(game* game);

void game_start(game* game);

void game_stop(game* game);

#endif
