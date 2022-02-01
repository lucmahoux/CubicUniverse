#ifndef CUB_UTILS_VECTOR_H
#define CUB_UTILS_VECTOR_H

#include "utils/utils.h"

typedef struct cub_vec3 {
    float coords[3];
} cub_vec3;

// Vector - Scalar representation of quaternions
typedef struct cub_quaternion {
    cub_vec3 vect;
    float w;
} cub_quaternion;

/* Return an initialised 3D vector (x, y, z) */
cub_vec3 cub_utils_vec3(float x, float y, float z);
float cub_utils_vec3_dot_product(cub_vec3* v1, cub_vec3* v2);
cub_vec3 cub_utils_vec3_cross_product(cub_vec3 v1, cub_vec3 v2);
cub_vec3 cub_utils_vec3_add(cub_vec3 v1, cub_vec3 v2);
cub_vec3 cub_utils_vec3_sub(cub_vec3 v1, cub_vec3 v2);
cub_vec3 cub_utils_vec3_scalar(float scalar, cub_vec3 v);
float cub_utils_vec3_magnitude(cub_vec3* v);
cub_vec3 cub_utils_vec3_normalize(cub_vec3 v);
cub_quaternion cub_utils_quaternion_normalize(cub_quaternion q);
cub_quaternion cub_utils_quaternion_conjugate(cub_quaternion q);
/* Rotates the 3D vector 'to_rotate' by 'angle' RADIANS around the rotation
 * axis 'axis'. The 3D vector axis SHOULD BE NORMALIZED in most cases ! */
cub_vec3 cub_utils_vec3_rotate(cub_vec3 to_rotate, cub_vec3 axis, float angle);

#endif
