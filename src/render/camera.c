#include "camera.h"
// ----------------------------------------------------------------------------
void frustum_setup_plane(struct cubPlane* plane, vec3 position, vec3 normal);

float get_signed_distance_to_plane(struct cubPlane* plane, vec3 point);

void frustum_update_vectors(ViewFrustum* frustum, float yaw, float pitch);

void frustum_update_planes(ViewFrustum* frustum, vec3 position);

/* Update the view matrix of the camera when the player 'moves' his head.
 * It is assumed that the relevant shader_program is being used upon calling */
void camera_update_view(Camera* camera);
// ----------------------------------------------------------------------------

// ---------- Camera part -------------

void camera_setup(Camera* camera, vec3 position, float FOV,
                  float aspect_ratio, float near_Z, float far_Z) {
    camera->view_matrix = mat4_view_matrix(
                            position,
                            VEC3(0.0f, 0.0f, 0.0f),
                            VEC3(0.0f, 1.0f, 0.0f)
                            );
    camera->frustum.FOV = FOV;
    camera->frustum.aspect_ratio = aspect_ratio;
    camera->frustum.near_Z = near_Z;
    camera->frustum.far_Z = far_Z;

    // New features for controlling camera
    camera->position = position;
    camera->frustum.world_up = VEC3(0.0f, 1.0f, 0.0f);
    camera->pitch = 0.0f;
    camera->yaw = -90.0f;
    camera->firstMouse = 1;
    camera->lastX = 1280 / 2.0f;
    camera->lastY = 720 / 2.0f;
    camera_update_projection(camera);
}

void camera_update_projection(Camera* camera) {
    camera->projection_matrix = mat4_perspective(camera->frustum.FOV,
                                                 camera->frustum.aspect_ratio,
                                                 camera->frustum.near_Z,
                                                 camera->frustum.far_Z);
    camera->VP_matrix = MAT4_PROD(camera->projection_matrix,
                                  camera->view_matrix);
}

void camera_update_view(Camera* camera) {
    frustum_update_vectors(&camera->frustum, camera->yaw, camera->pitch);
    frustum_update_planes(&camera->frustum, camera->position);
    camera->view_matrix = mat4_view_matrix(camera->position,
        VEC3_ADD(camera->position, camera->frustum.front),
        camera->frustum.world_up);
    camera->VP_matrix = MAT4_PROD(camera->projection_matrix,
                                  camera->view_matrix);
}

void camera_update(Camera* camera) {
    camera_update_view(camera);
}

// ------------ Frustum part ----------------
void frustum_update_vectors(ViewFrustum* frustum, float yaw, float pitch) {
    frustum->front = VEC3(cos(RAD(yaw)) * cos(RAD(pitch)),
                          sin(RAD(pitch)),
                          sin(RAD(yaw)) * cos(RAD(pitch)));
    frustum->front = VEC3_NORM(frustum->front);
    frustum->right = VEC3_NORM(VEC3_CROSS(frustum->front, frustum->world_up));
    frustum->up = VEC3_NORM(VEC3_CROSS(frustum->right, frustum->front));
}

void frustum_update_planes(ViewFrustum* frustum, vec3 position) {
    float H_far = frustum->far_Z * tan(frustum->FOV / 2.0f);
    float W_far = H_far * frustum->aspect_ratio;
    vec3 front_mult_far = VEC3_SCALE(frustum->far_Z, frustum->front);
    // Frustum in world space
    frustum_setup_plane(&frustum->near_plane, VEC3_ADD(position,
                        VEC3_SCALE(frustum->near_Z, frustum->front)),
                        frustum->front);
    frustum_setup_plane(&frustum->far_plane, VEC3_ADD(position,
                        front_mult_far), VEC3_SCALE(-1.0f, frustum->front));
    frustum_setup_plane(&frustum->right_plane, position,
                        VEC3_CROSS(frustum->up, VEC3_ADD(front_mult_far,
                        VEC3_SCALE(W_far, frustum->right))));
    frustum_setup_plane(&frustum->left_plane, position,
                        VEC3_CROSS(VEC3_SUB(front_mult_far,
                        VEC3_SCALE(W_far, frustum->right)), frustum->up));
    frustum_setup_plane(&frustum->bottom_plane, position,
                        VEC3_CROSS(frustum->right, VEC3_SUB(front_mult_far,
                        VEC3_SCALE(H_far, frustum->up))));
    frustum_setup_plane(&frustum->top_plane, position,
                        VEC3_CROSS(VEC3_ADD(front_mult_far,
                        VEC3_SCALE(H_far, frustum->up)), frustum->right));
}

void frustum_setup_plane(struct cubPlane* plane, vec3 position, vec3 normal) {
    plane->normal = VEC3_NORM(normal);
    plane->distance = VEC3_DOT(&plane->normal, &position);
}

float get_signed_distance_to_plane(struct cubPlane* plane, vec3 point) {
    return VEC3_DOT(&plane->normal, &point) - plane->distance;
}

bool is_sphere_in_frustum(ViewFrustum* frustum, vec3 center, float radius) {
    return get_signed_distance_to_plane(&frustum->left_plane, center) > -radius
        && get_signed_distance_to_plane(&frustum->right_plane, center) > -radius
        && get_signed_distance_to_plane(&frustum->far_plane, center) > -radius
        && get_signed_distance_to_plane(&frustum->near_plane, center) > -radius
        && get_signed_distance_to_plane(&frustum->top_plane, center) > -radius
        && get_signed_distance_to_plane(&frustum->bottom_plane, center) > -radius;
}
