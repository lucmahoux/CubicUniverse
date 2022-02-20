#ifndef CUB_RENDER_CAMERA_H
#define CUB_RENDER_CAMERA_H

#include "utils/utils.h"

typedef struct camera {
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
} camera;

/* Should be called once, at the beginning of the game to setup the camera */
void camera_setup(camera* camera, GLuint shader_program,
                            vec3 position, float fov, float aspect_ratio,
                             float near, float far);

/* Update the view matrix of the camera when the player 'moves' his head.
 * It is assumed that the relevant shader_program is being used upon calling */
void camera_update_view(camera* camera);

/* Update the projection matrix in the shader program when the window is
 * resized by the user */
void camera_update_projection(camera* camera,
                                         GLuint shader_program);

void camera_remove_translation(camera* camera);

void camera_update(camera* camera);

#endif
