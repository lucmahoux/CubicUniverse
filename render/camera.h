#ifndef CUB_RENDER_CAMERA_H
#define CUB_RENDER_CAMERA_H

#include "utils/utils.h"

/* Should be called once, at the beginning of the game to setup the camera */
void cub_render_setup_camera(cubCamera* camera, GLuint shader_program,
                             cubVec3 position, float fov, float aspect_ratio,
                             float near, float far);

/* Update the view matrix of the camera when the player 'moves' his head.
 * It is assumed that the relevant shader_program is being used upon calling */
void cub_render_update_camera_view(cubCamera* camera);

/* Update the projection matrix in the shader program when the window is
 * resized by the user */
void cub_render_update_camera_projection(cubCamera* camera,
                                         GLuint shader_program);

#endif
