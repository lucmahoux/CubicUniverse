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
        printf("Name: %s - ID: %i - Type: %s\n", block->name, block->id,
                block->type == CUBE_SAME_FACES ? "CUBE_SAME_FACES" :
                block->type == CUBE_UNIQUE_FACES ? "CUBE_UNIQUE_FACES" :
                "CUBE_TOP_BOTTOM");
    }
    cub_block_load_texture_pack(&game->block_renderer.block_list);
    
    cub_skybox_setup_renderer(&game->skybox_renderer);

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

// Change values with mouse input
void cub_game_process_mouse_mouvement(cubCamera* cam, float xoffset,
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

    cubVec3 direction;
    direction.coords[0] = cos(RAD(cam->yaw)) * cos(RAD(cam->pitch));
    direction.coords[1] = sin(RAD(cam->pitch));
    direction.coords[2] = sin(RAD(cam->yaw)) * cos(RAD(cam->pitch));
    cam->front = cub_utils_vec3_normalize(direction);

}

void cub_game_input_handler(cubGame* game) {
    // Exit program
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

    cub_game_process_mouse_mouvement(&game->camera, xoffset, yoffset);
}

void cub_game_skybox_render(cubGame* game)
{
    // Prepare skybox rendering
    glDepthFunc(GL_LEQUAL);
    glUseProgram(game->skybox_renderer.shader_program);
    cub_camera_remove_translation(&game->camera);
    game->camera.view_uni_loc = glGetUniformLocation(game->skybox_renderer.shader_program, "view");
    game->camera.projection_uni_loc = glGetUniformLocation(game->skybox_renderer.shader_program, "projection");
    
    // Render skybox
    glBindVertexArray(game->skybox_renderer.VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, game->skybox_renderer.cubemapTexture);
    glDrawArrays(GL_TRIANGLES,0 , 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void cub_game_renderer_handler(cubGame* game) {
    glUseProgram(game->block_renderer.shader_program);
    // Camera updates
    float currentFrame = glfwGetTime();
    game->camera.deltaTime = currentFrame - game->camera.lastFrame;
    game->camera.lastFrame = currentFrame;
    cub_render_update_camera_view(&game->camera);

    // World updates
    glBindVertexArray(game->block_renderer.VAO);
    cub_block_render(&game->block_renderer, 1, CUB_VEC3(0.0f, 0.0f, 0.0f));
    cub_block_render(&game->block_renderer, 2, CUB_VEC3(1.0f, 0.0f, 0.0f));
    cub_block_render(&game->block_renderer, 1, CUB_VEC3(0.0f, 0.0f, 1.0f));
    cub_block_render(&game->block_renderer, 2, CUB_VEC3(1.0f, 0.0f, 1.0f));
    cub_block_render(&game->block_renderer, 3, CUB_VEC3(1.0f, 1.0f, 1.0f));
    cub_block_render(&game->block_renderer, 3, CUB_VEC3(2.0f, 0.0f, 0.0f));

    //cub_chunk_render(&game->chunk_test, &game->block_renderer);
    
    cub_game_skybox_render(game);
}

void cub_game_start(cubGame* game) {
    //game->chunk_test = cub_chunk_create(0, 0);
    //cub_chunk_fill(&game->chunk_test, 1);
    cub_chunk_load(&game->chunk_test);
    cub_utils_start_window_loop(game->window,
                                (window_callback) cub_game_clear_screen_handler,
                                (window_callback) cub_game_input_handler,
                                (window_callback) cub_game_renderer_handler,
                                (window_callback) cub_game_stop,
                                (void*) game);
}

void cub_game_stop(cubGame* game) {
    // TODO: Save things on disk
    // cub_chunk_save(&game->chunk_test);
    cub_block_free_renderer(&game->block_renderer);
    cub_skybox_free_renderer(&game->skybox_renderer);
}
