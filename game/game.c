#include "game/game.h"
#include "utils/stb_image.h"

void cub_game_init(cubGame* game, int width, int height) {
    // Set up the window
    cub_utils_setup_window(&game->window, width, height, "Cubic Universe");
    stbi_set_flip_vertically_on_load(true);

    // Set up the renderer & load the texture pack
    cub_block_setup_renderer(&game->block_renderer);
    for (size_t i = 0; i < game->block_renderer.block_list.nb_blocks; ++i) {
        cubBlockData* block = &game->block_renderer.block_list.blocks[i];
        printf("Name: %s - ID: %u - RenderType: 0x%x - VAO_id: %u "
                "- is_solid: %u - has_gravity: %u - has_bs_tex: %u "
                "- is_bs_creator: %u - has_states: %u - nb_states: %u\n",
                block->name, block->id, block->render_type, block->VAO_id,
                block->block_info.is_solid, block->block_info.has_gravity,
                block->block_info.has_bs_tex, block->block_info.is_bs_creator,
                block->block_info.has_states, block->block_info.nb_states);
    }
    cub_block_load_texture_pack(&game->block_renderer.block_list);

    // Set up the camera
    float aspect_ratio = 1.0f * width / height;
    cub_render_setup_camera(&game->camera, game->block_renderer.shader_program,
                            CUB_VEC3(0.0f, 0.0f, 10.0f),
                            45.0f, aspect_ratio, 0.1f, 100.0f);
}

void cub_game_clear_screen_handler(cub_unused cubGame* game) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void cub_game_input_handler(cubGame* game) {
    if (glfwGetKey(game->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        cub_utils_close_window(game->window);

    // Moving commands
    cubCamera* cam = &game->camera;
    const float cameraSpeed = 2.5f * cam->deltaTime;
    if(glfwGetKey(game->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam->position = CUB_VEC3_ADD(
                        cam->position,
                        CUB_VEC3_SCALE(cameraSpeed, cam->front));
    }
    if(glfwGetKey(game->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam->position = CUB_VEC3_SUB(
                cam->position,
                CUB_VEC3_SCALE(cameraSpeed, cam->front));
    }
    if(glfwGetKey(game->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam->position = CUB_VEC3_SUB(
                cam->position,
                CUB_VEC3_SCALE(cameraSpeed,
                    CUB_VEC3_NORM(CUB_VEC3_CROSS(cam->front,cam->up_side))));
    }
    if(glfwGetKey(game->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam->position = CUB_VEC3_ADD(
                cam->position,
                CUB_VEC3_SCALE(cameraSpeed,
                    CUB_VEC3_NORM(CUB_VEC3_CROSS(cam->front,cam->up_side))));
    }
}

void cub_game_renderer_handler(cubGame* game) {
    glUseProgram(game->block_renderer.shader_program);
    // Camera updates
    float currentFrame = glfwGetTime();
    game->camera.deltaTime = currentFrame - game->camera.lastFrame;
    game->camera.lastFrame = currentFrame;
    cub_render_update_camera_view(&game->camera);

    // World updates
    glBindVertexArray(game->block_renderer.buffer_objs[CUB_DEFAULT_VAO_ID].VAO);
    /*cubBlockState bs = { .id = 1, .states = NULL };
    cub_bs_val bs_vals[3] = { 0, 0, 0 };
    bs.states = bs_vals;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(0.0f, 0.0f, -2.0f));
    bs.id = 2;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(0.0f, 0.0f, 2.0f));
    bs.id = 3;
    bs.states[0] = BSV_OAK;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(0.0f, 2.0f, 0.0f));
    bs.states[0] = BSV_ACACIA;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(0.0f, 0.0f, 0.0f));
    bs.states[0] = BSV_SPRUCE;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(0.0f, -2.0f, 0.0f));
    bs.id = 4;
    bs.states[0] = BSV_OAK;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(1.0f, 1.0f, 0.0f));
    bs.states[0] = BSV_ACACIA;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(0.0f, 1.0f, 1.0f));
    bs.states[0] = BSV_SPRUCE;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(1.0f, 0.0f, 1.0f));
    bs.id = 5;
    bs.states[0] = BSV_POPPY;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(0.0f, 3.0f, 0.0f));
    bs.states[0] = BSV_TULIP_ORANGE;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(1.0f, 3.0f, 0.0f));
    bs.states[0] = BSV_LILY_OF_THE_VALLEY;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(2.0f, 3.0f, 0.0f));
    bs.states[0] = BSV_OXEYE;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(3.0f, 3.0f, 0.0f));
    bs.id = 6;
    bs.states[0] = BSV_FALSE;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(4.0f, 3.0f, 0.0f));
    bs.states[0] = BSV_TRUE;
    cub_block_render(&game->block_renderer, &bs,
                        CUB_VEC3(5.0f, 3.0f, 0.0f));*/
    cub_chunk_render(&game->chunk_test, &game->block_renderer);
}

void cub_game_start(cubGame* game) {
    /*game->chunk_test = cub_chunk_create(0, 0);
    cubBlockState bs;
    bs.id = 5;
    cub_bs_val states[3];
    bs.states = states;
    states[0] = BSV_POPPY;
    cub_chunk_fill(&game->block_renderer, &game->chunk_test, &bs);*/
    cub_chunk_load(&game->chunk_test, &game->block_renderer);
    cub_utils_start_window_loop(game->window,
                                (window_callback) cub_game_clear_screen_handler,
                                (window_callback) cub_game_input_handler,
                                (window_callback) cub_game_renderer_handler,
                                (window_callback) cub_game_stop,
                                (void*) game);
}

void cub_game_stop(cubGame* game) {
    // TODO: Save things on disk
    cub_chunk_save(&game->chunk_test, &game->block_renderer);
    cub_block_free_renderer(&game->block_renderer);
}
