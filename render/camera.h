#ifndef CUB_RENDER_CAMERA_H
#define CUB_RENDER_CAMERA_H

#include "utils/utils.h"

typedef struct cubCamera {
    GLint view_uni_loc; // Uniform location of view matrix
    mat4 view_matrix;
    GLint projection_uni_loc; // Uniform location of projection matrix
    mat4 projection_matrix;
    float fov; // Field Of View
    float aspect_ratio; // Aspect ration = width / height
    float near, far; // Near and Far planes distances
    vec3 position;
    vec3 up_side;
    vec3 front;
    float deltaTime;
    float lastFrame;
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    int firstMouse;
} cubCamera;

/* Should be called once, at the beginning of the game to setup the camera */
void cub_render_setup_camera(cubCamera* camera, GLuint shader_program,
                            vec3 position, float fov, float aspect_ratio,
                             float near, float far);

/* Update the view matrix of the camera when the player 'moves' his head.
 * It is assumed that the relevant shader_program is being used upon calling */
void cub_render_update_camera_view(cubCamera* camera);

/* Update the projection matrix in the shader program when the window is
 * resized by the user */
void cub_render_update_camera_projection(cubCamera* camera,
                                         GLuint shader_program);

void cub_camera_remove_translation(cubCamera* camera);

void cub_render_update_camera(cubCamera* camera);

#endif
