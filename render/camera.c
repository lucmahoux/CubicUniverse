#include "render/camera.h"

void cub_render_setup_camera(cubCamera* camera, GLuint shader_program,
                             cubVec3 position, float fov, float aspect_ratio,
                             float near, float far) {
    camera->view_uni_loc = glGetUniformLocation(shader_program, "view_matrix");
    camera->view_matrix = cub_utils_mat4_view_matrix(position,
                                                CUB_VEC3(0.0f, 0.0f, 0.0f),
                                                CUB_VEC3(0.0f, 1.0f, 0.0f));
    camera->projection_uni_loc = glGetUniformLocation(shader_program,
                                                      "projection_matrix");
    camera->fov = fov;
    camera->aspect_ratio = aspect_ratio;
    camera->near = near;
    camera->far = far;

    // New features for controlling camerA
    camera->position = position;
    camera->up_side = CUB_VEC3(0.0f,1.0f,0.0f);
    camera->front = CUB_VEC3(0.0f,0.0f,-1.0f);
    camera->deltaTime = 0.0f;
    camera->lastFrame = 0.0f;
    cub_render_update_camera_projection(camera, shader_program);
}

void cub_render_update_camera_projection(cubCamera* camera,
                                         GLuint shader_program) {
    glUseProgram(shader_program);
    camera->projection_matrix = cub_utils_mat4_perspective(
                                    camera->fov, camera->aspect_ratio,
                                    camera->near, camera->far);
    glUniformMatrix4fv(camera->projection_uni_loc, 1, GL_FALSE,
                        camera->projection_matrix.coeffs);
}

void cub_render_update_camera_view(cubCamera* camera) {
    /*camera->view_matrix = cub_utils_mat4_view_matrix(
            camera->position,
            cub_utils_vec3_add(camera->position,camera->front),
            camera->up_side);*/
    float camX = sin(glfwGetTime()) * 10.0f;
    float camZ = cos(glfwGetTime()) * 10.0f;
    camera->view_matrix = cub_utils_mat4_view_matrix(
            CUB_VEC3(camX, 0.0f, camZ),
            CUB_VEC3(0.0f, 0.0f, 0.0f),
            CUB_VEC3(0.0f, 2.0f, 0.0f));
    glUniformMatrix4fv(camera->view_uni_loc, 1, GL_FALSE,
                        camera->view_matrix.coeffs);
}
