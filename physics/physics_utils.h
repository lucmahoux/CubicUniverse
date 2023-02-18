#ifndef CUB_PHYSICS_PHYSICS_UTILS_H
#define CUB_PHYSICS_PHYSICS_UTILS_H

#include "utils/utils.h"
#include "utils/vector.h"

typedef struct cubBoxCollider {
    vec3 min;
    vec3 max;
} BoxCollider;


void physics_check_collision(BoxCollider* A, BoxCollider* B);

void physics_set_box_collider(BoxCollider* collider, float* vertices,
                              size_t nb_vertices, size_t stride);

#endif
