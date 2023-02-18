#ifndef CUB_PHYSICS_PHYSICS_UTILS_H
#define CUB_PHYSICS_PHYSICS_UTILS_H

#include "utils.h"
#include "data_struct/vector.h"

typedef struct cubBoxCollider {
    vec3 min;
    vec3 max;
} BoxCollider;


bool physics_check_collision(BoxCollider* A, vec3 posA,
                             BoxCollider* B, vec3 posB);

void physics_set_box_collider(BoxCollider* collider, float* vertices,
                              size_t nb_vertices, size_t stride);

#endif
