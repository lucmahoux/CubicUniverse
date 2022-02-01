#ifndef CUB_UTILS_VECTOR_H
#define CUB_UTILS_VECTOR_H

#include "utils/utils.h"

#define CUB_VEC3(x, y, z) cub_utils_vec3(x, y, z)
#define CUB_VEC3_CROSS(u, v) cub_utils_vec3_cross_product(u, v)
#define CUB_VEC3_DOT(ptr_u, ptr_v) cub_utils_vec3_dot_product(ptr_u, ptr_v)
#define CUB_VEC3_ADD(u, v) cub_utils_vec3_add(u, v)
#define CUB_VEC3_SUB(u, v) cub_utils_vec3_sub(u, v)
#define CUB_VEC3_SCALE(k, u) cub_utils_vec3_scalar(k, u)
#define CUB_VEC3_MAG(ptr_u) cub_utils_vec3_magnitude(ptr_u)
#define CUB_VEC3_NORM(u) cub_utils_vec3_normalize(u)
#define CUB_QUAT_NORM(q) cub_utils_quaternion_normalize(q)
#define CUB_QUAT_CONJ(q) cub_utils_quaternion_conjugate(q)
#define CUB_QUAT_PROD(q1, q2) cub_utils_quaternion_product(q1, q2)
#define CUB_VEC3_ROT(u, axis, theta) cub_utils_vec3_rotate(u, axis, theta)

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
cub_quaternion cub_utils_quaternion_product(cub_quaternion q1,
                                            cub_quaternion q2);
/* Rotates the 3D vector 'to_rotate' by 'angle' RADIANS around the rotation
 * axis 'axis'. The 3D vector axis SHOULD BE NORMALIZED in most cases ! */
cub_vec3 cub_utils_vec3_rotate(cub_vec3 to_rotate, cub_vec3 axis, float angle);

#endif
