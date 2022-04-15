#ifndef CUB_GAME_GAME_H
#define CUB_GAME_GAME_H

#include "block/block.h"
#include "chunk/chunk.h"
#include "chunk/chunk_loader.h"
#include "chunk/chunk_renderer.h"
#include "render/axes.h"
#include "render/camera.h"
#include "render/line_renderer.h"
#include "render/skybox.h"
#include "render/open_gl.h"
#include "render/window.h"
#include "utils/utils.h"

typedef struct cubFpsCounter {
    float delta_time;
    float last_frame;
    float delta_time_since_fps_update;
    size_t nb_frames_since_fps_update;
    uint16_t fps;
} FpsCounter;

typedef struct cubGame {
    GLFWwindow* window;
    BlockList block_list;
    BlockRenderer block_renderer;
    ChunkRenderer chunk_renderer;
    SkyboxRenderer skybox_renderer;
    AxesRenderer axes_renderer;
    LineRenderer line_renderer;
    FpsCounter fps_counter;
    Camera camera;
    Chunk chunk_test;
} Game;

void game_init(Game* game, int width, int height);

void game_clear_screen_handler(Game* game);

void game_input_handler(Game* game);

void game_fps_counter_update(FpsCounter* fps_counter);

void game_renderer_handler(Game* game);

void game_start(Game* game);

void game_stop(Game* game);

#endif
