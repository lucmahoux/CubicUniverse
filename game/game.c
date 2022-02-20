#include "game/game.h"
#include "utils/stb_image.h"

void game_init(game* game, int width, int height) {
    // Set up the window
    setup_window(&game->window, width, height, "Cubic Universe");
    stbi_set_flip_vertically_on_load(true);

    // Set up the renderer & load the texture pack
    block_setup_renderer(&game->block_renderer);
    for (size_t i = 0; i < game->block_renderer.block_list.nb_blocks; ++i) {
        blockData* block = &game->block_renderer.block_list.blocks[i];
        printf("Name: %s - ID: %u - RenderType: 0x%x - VAO_id: %u "
                "- is_solid: %u - has_gravity: %u - has_bs_tex: %u "
                "- is_bs_creator: %u - has_states: %u - nb_states: %u\n",
                block->name, block->id, block->render_type, block->VAO_id,
                block->block_info.is_solid, block->block_info.has_gravity,
                block->block_info.has_bs_tex, block->block_info.is_bs_creator,
                block->block_info.has_states, block->block_info.nb_states);
    }
    block_load_texture_pack(&game->block_renderer.block_list);
    
    cub_skybox_setup_renderer(&game->skybox_renderer);

    // Set up the camera
    float aspect_ratio = 1.0f * width / height;
    camera_setup(&game->camera, game->block_renderer.shader_program,
                            VEC3(0.0f, 0.0f, 10.0f),
                            45.0f, aspect_ratio, 0.1f, 100.0f);
}

void game_clear_screen_handler(cub_unused game* game) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Change values with mouse input
void game_process_mouse_mouvement(camera* cam, float xoffset,
        float yoffset)
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

    vec3 direction;
    direction.coords[0] = cos(RAD(cam->yaw)) * cos(RAD(cam->pitch));
    direction.coords[1] = sin(RAD(cam->pitch));
    direction.coords[2] = sin(RAD(cam->yaw)) * cos(RAD(cam->pitch));
    cam->front = vec3_normalize(direction);

}

void game_input_handler(game* game) {
    // Exit program
    if (glfwGetKey(game->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        close_window(game->window);

    // Moving commands
    camera* cam = &game->camera;
    const float cameraSpeed = 2.5f * cam->deltaTime;
    if(glfwGetKey(game->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam->position = VEC3_ADD(
                        cam->position,
                        VEC3_SCALE(cameraSpeed, cam->front));
    }
    if(glfwGetKey(game->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam->position = VEC3_SUB(
                cam->position,
                VEC3_SCALE(cameraSpeed, cam->front));
    }
    if(glfwGetKey(game->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam->position = VEC3_SUB(
                cam->position,
               VEC3_SCALE(cameraSpeed,
                    VEC3_NORM(VEC3_CROSS(cam->front,cam->up_side))));
    }
    if(glfwGetKey(game->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam->position = VEC3_ADD(
                cam->position,
                VEC3_SCALE(cameraSpeed,
                    VEC3_NORM(VEC3_CROSS(cam->front,cam->up_side))));
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

void game_skybox_render(game* game)
{
    // Prepare skybox rendering
    //glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glUseProgram(game->skybox_renderer.shader_program);
    //cub_camera_remove_translation(&game->camera);
    mat4 view = MAT4_TRANS(game->camera.view_matrix,
            VEC3(0.0,0.0,0.0));
    glUniformMatrix4fv(game->skybox_renderer.view_uni_loc, 1, GL_FALSE,
           view.coeffs);
    glUniformMatrix4fv(game->skybox_renderer.projection_uni_loc, 1, GL_FALSE,
           game->camera.projection_matrix.coeffs);

    // Render skybox
    glBindVertexArray(game->skybox_renderer.VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, game->skybox_renderer.cubemapTexture);
    glDrawArrays(GL_TRIANGLES,0 , 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void game_renderer_handler(game* game) {
    glUseProgram(game->block_renderer.shader_program);
    // Camera updates
    camera_update(&game->camera);

    // World updates
    glBindVertexArray(game->block_renderer.buffer_objs[DEFAULT_VAO_ID].VAO);
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

    chunk_render(&game->chunk_test, &game->block_renderer);
    game_skybox_render(game);
}

void game_start(game* game) {
    /*game->chunk_test = cub_chunk_create(0, 0);
    cubBlockState bs;
    bs.id = 5;
    cub_bs_val states[3];
    bs.states = states;
    states[0] = BSV_POPPY;
    cub_chunk_fill(&game->block_renderer, &game->chunk_test, &bs);*/
    chunk_load(&game->chunk_test, &game->block_renderer);
    start_window_loop(game->window,
                                (window_callback) game_clear_screen_handler,
                                (window_callback) game_input_handler,
                                (window_callback) game_renderer_handler,
                                (window_callback) game_stop,
                                (void*) game);
}

void game_stop(game* game) {
    // TODO: Save things on disk
    chunk_save(&game->chunk_test, &game->block_renderer);
    block_free_renderer(&game->block_renderer);
    cub_skybox_free_renderer(&game->skybox_renderer);
}
