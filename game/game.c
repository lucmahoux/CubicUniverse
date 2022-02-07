#include "game/game.h"

void cub_game_init(cubGame* game, int width, int height) {
    // Set up the window
    cub_utils_setup_window(&game->window, width, height, "Cubic Universe");

    // Set up the renderer & load the texture pack
    cub_block_setup_renderer(&game->block_renderer);
    for (size_t i = 0; i < game->block_renderer.block_list.nb_blocks; ++i) {
        cubBlockData* block = &game->block_renderer.block_list.blocks[i];
        printf("Name: %s - ID: %i - Type: %s\n", block->name, block->id,
                block->type == CUBE_SAME_FACES ? "CUBE_SAME_FACES" :
                block->type == CUBE_UNIQUE_FACES ? "CUBE_UNIQUE_FACES" :
                "CUBE_TOP_BOTTOM");
    }
    cub_block_load_texture_pack(&game->block_renderer.block_list);

    // Set up the camera
    float aspect_ratio = 1.0f * width / height;
    cub_render_setup_camera(&game->camera, game->block_renderer.shader_program,
                            CUB_VEC3(0.0f, 0.0f, -3.0f),
                            45.0f, aspect_ratio, 0.1f, 100.0f);
}

void cub_game_clear_screen_handler(cub_unused cubGame* game) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void cub_game_input_handler(cubGame* game) {
    if (glfwGetKey(game->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        cub_utils_close_window(game->window);
}

void cub_game_renderer_handler(cubGame* game) {
    glUseProgram(game->block_renderer.shader_program);
    // Camera updates
    cub_render_update_camera_view(&game->camera);
    // World updates
    glBindVertexArray(game->block_renderer.VAO);
    cub_block_render(&game->block_renderer, 0, CUB_VEC3(0.0f, 0.0f, 0.0f));
    cub_block_render(&game->block_renderer, 1, CUB_VEC3(2.0f, 0.0f, 0.0f));
}

void cub_game_start(cubGame* game) {
    cub_utils_start_window_loop(game->window,
                                (window_callback) cub_game_clear_screen_handler,
                                (window_callback) cub_game_input_handler,
                                (window_callback) cub_game_renderer_handler,
                                (window_callback) cub_game_stop,
                                (void*) game);
}

void cub_game_stop(cubGame* game) {
    // TODO: Save things on disk
    cub_block_free_renderer(&game->block_renderer);
}