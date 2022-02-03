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

typedef struct cubVec3 {
    float coords[3];
} cubVec3;

// Vector - Scalar representation of quaternions
typedef struct cubQuaternion {
    cubVec3 vect;
    float w;
} cubQuaternion;

/* Return an initialised 3D vector (x, y, z) */
cubVec3 cub_utils_vec3(float x, float y, float z);
float cub_utils_vec3_dot_product(cubVec3* v1, cubVec3* v2);
cubVec3 cub_utils_vec3_cross_product(cubVec3 v1, cubVec3 v2);
cubVec3 cub_utils_vec3_add(cubVec3 v1, cubVec3 v2);
cubVec3 cub_utils_vec3_sub(cubVec3 v1, cubVec3 v2);
cubVec3 cub_utils_vec3_scalar(float scalar, cubVec3 v);
float cub_utils_vec3_magnitude(cubVec3* v);
cubVec3 cub_utils_vec3_normalize(cubVec3 v);
cubQuaternion cub_utils_quaternion_normalize(cubQuaternion q);
cubQuaternion cub_utils_quaternion_conjugate(cubQuaternion q);
cubQuaternion cub_utils_quaternion_product(cubQuaternion q1,
                                            cubQuaternion q2);
/* Rotates the 3D vector 'to_rotate' by 'angle' RADIANS around the rotation
 * axis 'axis'. The 3D vector axis SHOULD BE NORMALIZED in most cases ! */
cubVec3 cub_utils_vec3_rotate(cubVec3 to_rotate, cubVec3 axis, float angle);

#endif
