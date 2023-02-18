#include "game.h"
#include "stb_image.h"

void print_block_list(BlockList* block_list) {
    printf("*--*-- Block List --*--*\n");
    for (size_t i = 0; i < block_list->nb_blocks; ++i) {
        BlockData* block = block_list->blocks + i;
        printf( "  * %s [ID = %u] \n"
                "\t-> RenderType: 0x%x  [info_RBO_id = %u]\n"
                "\t-> is_solid: %u      | has_gravity: %u   | is_opaque %u\n"
                "\t-> has_bs_tex: %u    | is_bs_creator: %u | has_states: %u\n"
                "\t-> nb_states: %u     | origin_BS_uid: %u\n",
                block->name, block->id, block->render_type, block->info_RBO_id,
                block->flags.is_solid, block->flags.has_gravity,
                block->flags.is_opaque, block->flags.has_bs_tex,
                block->flags.is_bs_creator, block->flags.has_states,
                block->nb_states, block->origin_BS_uid);
    }
    printf("*--*-- Block List End --*--*\n");
}

void game_init(Game* game, int width, int height) {
    // Set up the window
    setup_window(&game->window, width, height, "Cubic Universe");

    // Set up the camera
    float aspect_ratio = 1.0f * width / height;
    camera_setup(&game->camera, VEC3(0.0f, 0.0f, 10.0f),
                 RAD(90.0f), aspect_ratio, 0.1f, 100.0f);

    // Set up the renderer & load the texture pack
    block_setup_renderer_and_block_list(&game->block_renderer,
                                        &game->block_list);

    print_block_list(&game->block_list);
    skybox_setup_renderer(&game->skybox_renderer);
    axes_setup_renderer(&game->axes_renderer);
    line_renderer_setup(&game->line_renderer);
    chunk_renderer_setup(&game->chunk_renderer, &game->block_list,
                         game->camera.position, 3);

    // Set up the fps_counter
    game->fps_counter.delta_time = 0.0f;
    game->fps_counter.last_frame = 0.0f;
    game->fps_counter.delta_time_since_fps_update = 0.0f;
    game->fps_counter.nb_frames_since_fps_update = 0;
    game->fps_counter.fps = 0;
}

void game_clear_screen_handler(cub_unused Game* game) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Change values with mouse input
void game_process_mouse_mouvement(Camera* cam, float xoffset, float yoffset)
{
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cam->yaw += xoffset;
    cam->pitch += yoffset;

    if(cam->pitch > 89.0f)
        cam->pitch = 89.0f;
    if(cam->pitch < -89.0f)
        cam->pitch = -89.0f;
}

void game_input_handler(Game* game) {
    // Exit program
    if (glfwGetKey(game->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        close_window(game->window);

    // Moving commands
    Camera* cam = &game->camera;
    const float cameraSpeed = 10.0f * game->fps_counter.delta_time;
    if(glfwGetKey(game->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam->position = VEC3_ADD(cam->position,
                        VEC3_SCALE(cameraSpeed, cam->frustum.front));
    }
    if(glfwGetKey(game->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam->position = VEC3_SUB(cam->position,
                        VEC3_SCALE(cameraSpeed, cam->frustum.front));
    }
    if(glfwGetKey(game->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam->position = VEC3_SUB(cam->position,
                        VEC3_SCALE(cameraSpeed, cam->frustum.right));
    }
    if(glfwGetKey(game->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam->position = VEC3_ADD(cam->position,
                        VEC3_SCALE(cameraSpeed, cam->frustum.right));
    }

    // Mouse movements
    double xpos, ypos;
    glfwGetCursorPos(game->window, &xpos, &ypos);

    if(cam->firstMouse == 1)
    {
        cam->lastX = xpos;
        cam->lastY = ypos;
        cam->firstMouse = 0;
    }

    float xoffset = xpos - cam->lastX;
    float yoffset = cam->lastY - ypos;
    cam->lastX = xpos;
    cam->lastY = ypos;

    game_process_mouse_mouvement(&game->camera, xoffset, yoffset);
}

void game_fps_counter_update(FpsCounter* fps_counter) {
    float current_frame = glfwGetTime();
    if (fps_counter->delta_time_since_fps_update >= 1.0f) {
        fps_counter->fps = (uint16_t)(
                (float)fps_counter->nb_frames_since_fps_update /
                fps_counter->delta_time_since_fps_update
        );
        fps_counter->nb_frames_since_fps_update = 0;
        fps_counter->delta_time_since_fps_update = 0.0f;
        printf("FPS counter: %u\n", fps_counter->fps);
    }
    ++fps_counter->nb_frames_since_fps_update;
    fps_counter->delta_time = current_frame - fps_counter->last_frame;
    fps_counter->last_frame = current_frame;
    fps_counter->delta_time_since_fps_update += fps_counter->delta_time;
}

void render_block(Game* game, BlockState* BS, SubChunkCoords sc_coords) {
    block_set_BS_uid(BS);
    block_cubic_add_to_cache(&game->block_renderer.cubic,
                             BS, FID_TOP, sc_coords);
    block_cubic_add_to_cache(&game->block_renderer.cubic,
                             BS, FID_BOTTOM, sc_coords);
    block_cubic_add_to_cache(&game->block_renderer.cubic,
                             BS, FID_BACK, sc_coords);
    block_cubic_add_to_cache(&game->block_renderer.cubic,
                             BS, FID_FRONT, sc_coords);
    block_cubic_add_to_cache(&game->block_renderer.cubic,
                             BS, FID_LEFT, sc_coords);
    block_cubic_add_to_cache(&game->block_renderer.cubic,
                             BS, FID_RIGHT, sc_coords);
}

#define GET_BLOCK_DATA_FROM_ID(block_list, ID) (block_list.blocks + ID)

void game_renderer_handler(Game* game) {
    // Camera updates
    game_fps_counter_update(&game->fps_counter);
    camera_update(&game->camera);
    glUseProgram(game->line_renderer.shader_program);
    glUniformMatrix4fv(game->line_renderer.VP_matrix_loc,
                       1, GL_FALSE, game->camera.VP_matrix.coeffs);
    glUseProgram(game->block_renderer.cubic.shader);
    glUniformMatrix4fv(game->block_renderer.cubic.uniforms.VP_matrix,
                       1, GL_FALSE, game->camera.VP_matrix.coeffs);
    glUseProgram(game->block_renderer.custom.shader);
    glUniformMatrix4fv(game->block_renderer.custom.uniforms.VP_matrix,
                       1, GL_FALSE, game->camera.VP_matrix.coeffs);

    // World updates
    /*BlockState bs = { .block = GET_BLOCK_DATA_FROM_ID(game->block_list, 1),
                      .states = NULL };
    SubChunkCoords sc_coords = { .x = 0, .y = 0, .z = 0 };
    bs_val bs_vals[3] = { 0, 0, 0 };
    bs.states = bs_vals;
    render_block(game, &bs, sc_coords);

    bs.block = GET_BLOCK_DATA_FROM_ID(game->block_list, 2);
    sc_coords.y = 2;
    render_block(game, &bs, sc_coords);

    bs.block = GET_BLOCK_DATA_FROM_ID(game->block_list, 3);
    bs.states[0] = BSV_OAK;
    sc_coords.z = 2;
    render_block(game, &bs, sc_coords);

    bs.states[0] = BSV_ACACIA;
    sc_coords.x = 2;
    render_block(game, &bs, sc_coords);

    bs.states[0] = BSV_SPRUCE;
    sc_coords.y = 4;
    render_block(game, &bs, sc_coords);

    bs.block = GET_BLOCK_DATA_FROM_ID(game->block_list, 4);
    bs.states[0] = BSV_OAK;
    sc_coords.z = 4;
    render_block(game, &bs, sc_coords);

    bs.states[0] = BSV_ACACIA;
    sc_coords.x = 0;
    render_block(game, &bs, sc_coords);

    bs.states[0] = BSV_SPRUCE;
    sc_coords.z = 0;
    render_block(game, &bs, sc_coords);

    sc_coords.y = 15;
    bs.block = GET_BLOCK_DATA_FROM_ID(game->block_list, 5);
    bs.states[0] = BSV_POPPY;
    block_set_BS_uid(&bs);
    block_custom_render(&game->block_renderer.custom, &bs, sc_coords);
    sc_coords.y = 14;
    bs.states[0] = BSV_TULIP_ORANGE;
    block_set_BS_uid(&bs);
    block_custom_render(&game->block_renderer.custom, &bs, sc_coords);
    sc_coords.y = 13;
    bs.states[0] = BSV_LILY_OF_THE_VALLEY;
    block_set_BS_uid(&bs);
    block_custom_render(&game->block_renderer.custom, &bs, sc_coords);
    sc_coords.y = 12;
    bs.states[0] = BSV_OXEYE;
    block_set_BS_uid(&bs)
    block_custom_render(&game->block_renderer.custom, &bs, sc_coords);

    bs.block = GET_BLOCK_DATA_FROM_ID(game->block_list, 6);
    bs.states[0] = BSV_FALSE;
    sc_coords.x = 2;
    render_block(game, &bs, sc_coords);

    bs.states[0] = BSV_TRUE;
    sc_coords.z = 6;
    render_block(game, &bs, sc_coords);

    block_cubic_flush_cache(&game->block_renderer.cubic);*/
    chunk_renderer_update(&game->chunk_renderer, &game->block_list,
                          game->camera.position);
    chunk_renderer_render(&game->chunk_renderer, &game->block_renderer,
                          &game->camera);
    skybox_render(&game->skybox_renderer, &game->camera);
    axes_render(&game->axes_renderer, &game->camera);
}

void game_start(Game* game) {
    start_window_loop(game->window,
                      (window_callback) game_clear_screen_handler,
                      (window_callback) game_input_handler,
                      (window_callback) game_renderer_handler,
                      (window_callback) game_stop,
                      (void*) game);
}

void game_stop(Game* game) {
    // TODO: Save things on disk -> SAVE CHUNK GRID
    chunk_renderer_free(&game->chunk_renderer);
    block_free_renderer(&game->block_renderer);
    skybox_free_renderer(&game->skybox_renderer);
    axes_free_renderer(&game->axes_renderer);
    line_renderer_free(&game->line_renderer);
    BLloader_free_block_list(&game->block_list);
}
