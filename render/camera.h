#ifndef CUB_RENDER_CAMERA_H
#define CUB_RENDER_CAMERA_H

#include "utils/utils.h"

struct cubPlane {
    vec3 normal; // Coordinates (a, b, c)
    float distance; // Plane equation is ax + by + cz + distance = 0
};

typedef struct cubViewFrustum {
    float FOV; // Vertical FOV
    float aspect_ratio; // = width / height
    float near_Z, far_Z;
    vec3 right, up, front; // Base vectors
    vec3 world_up;
    struct cubPlane near_plane, far_plane, left_plane,
                    right_plane, top_plane, bottom_plane;
} ViewFrustum;

typedef struct cubCamera {
    mat4 view_matrix;
    mat4 projection_matrix;
    mat4 VP_matrix; // VP_matrix = projection_matrix x view_matrix
    ViewFrustum frustum;
    vec3 position;
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    int firstMouse;
} Camera;

/* Should be called once, at the beginning of the game to setup the camera */
void camera_setup(Camera* camera, vec3 position, float fov,
                  float aspect_ratio, float near, float far);

/* Update the projection matrix (e.g. when window is resized by the user) */
void camera_update_projection(Camera* camera);

void camera_update(Camera* camera);

bool is_sphere_in_frustum(ViewFrustum* frustum, vec3 center, float radius);

#endif
