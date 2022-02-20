#include "render/camera.h"

void camera_setup(camera* camera, GLuint shader_program,
                             vec3 position, float fov, float aspect_ratio,
                             float near, float far) {
    camera->view_uni_loc = glGetUniformLocation(shader_program, "view_matrix");
    camera->view_matrix = mat4_view_matrix(position,
                                                VEC3(0.0f, 0.0f, 0.0f),
                                                VEC3(0.0f, 1.0f, 0.0f));
    camera->projection_uni_loc = glGetUniformLocation(shader_program,
                                                      "projection_matrix");
    camera->fov = fov;
    camera->aspect_ratio = aspect_ratio;
    camera->near = near;
    camera->far = far;

    // New features for controlling camerA
    camera->position = position;
    camera->up_side = VEC3(0.0f,1.0f,0.0f);
    camera->front = VEC3(0.0f,0.0f,-1.0f);
    camera->deltaTime = 0.0f;
    camera->lastFrame = 0.0f;
    camera->pitch = 0.0f;
    camera->yaw = -90.0f;
    camera->firstMouse = 1;
    camera->lastX = 1280 / 2.0f;
    camera->lastY = 720 / 2.0f;
    camera_update_projection(camera, shader_program);
}

void camera_update_projection(camera* camera,
                                         GLuint shader_program) {
    glUseProgram(shader_program);
    camera->projection_matrix = mat4_perspective(
                                    camera->fov, camera->aspect_ratio,
                                    camera->near, camera->far);
    glUniformMatrix4fv(camera->projection_uni_loc, 1, GL_FALSE,
                        camera->projection_matrix.coeffs);
}

void camera_update_view(camera* camera) {
    camera->view_matrix = mat4_view_matrix(
            camera->position,
            vec3_add(camera->position,camera->front),
            camera->up_side);
    glUniformMatrix4fv(camera->view_uni_loc, 1, GL_FALSE,
                        camera->view_matrix.coeffs);
}

void camera_remove_translation(camera* camera)
{
    camera->view_matrix.coeffs[3] = 0;
    camera->view_matrix.coeffs[7] = 0;
    camera->view_matrix.coeffs[11] = 0;
    camera->view_matrix.coeffs[12] = 0;
    camera->view_matrix.coeffs[13] = 0;
    camera->view_matrix.coeffs[14] = 0;
    camera->view_matrix.coeffs[15] = 0;
}

void camera_update_time(camera* camera)
{
    float currentFrame = glfwGetTime();
    camera->deltaTime = currentFrame - camera->lastFrame;
    camera->lastFrame = currentFrame;
}

void camera_update(camera* camera)
{
    camera_update_time(camera);
    camera_update_view(camera);
}
