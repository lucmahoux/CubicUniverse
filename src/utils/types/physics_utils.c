#include "physics_utils.h"

bool physics_check_collision(BoxCollider* A, vec3 posA,
                             BoxCollider* B, vec3 posB) {
    vec3 minA = VEC3_ADD(A->min, posA),
         maxA = VEC3_ADD(A->max, posA),
         minB = VEC3_ADD(B->min, posB),
         maxB = VEC3_ADD(B->max, posB);
    return minA.coords[0] <= maxB.coords[0] && minB.coords[0] <= maxA.coords[0]
        && minA.coords[1] <= maxB.coords[1] && minB.coords[1] <= maxA.coords[1]
        && minA.coords[2] <= maxB.coords[2] && minB.coords[2] <= maxA.coords[2];
}

void physics_set_box_collider(BoxCollider* collider, float* vertices,
                              size_t nb_vertices, size_t stride) {
    if (nb_vertices == 0)
        errx(EXIT_FAILURE, "physics_set_box_collider: no vertex found!");
    collider->min = VEC3(vertices[0], vertices[1], vertices[2]);
    collider->max = collider->min;
    for (size_t i = 1; i < nb_vertices; ++i) {
        vertices += stride;
        for (uint8_t j = 0; j < 3; ++j) {
            if (vertices[j] < collider->min.coords[j])
                collider->min.coords[j] = vertices[j];
            else if (vertices[j] > collider->max.coords[j])
                collider->max.coords[j] = vertices[j];
        }
    }
}
