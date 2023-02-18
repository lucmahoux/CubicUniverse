#ifndef CUB_UTILS_VECTOR_H
#define CUB_UTILS_VECTOR_H

#include <err.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VEC3(x, y, z) vec3_new(x, y, z)
#define VEC3_CONST(X, Y, Z) { .coords = {X, Y, Z} }
#define VEC3_CROSS(u, v) vec3_cross_product(u, v)
#define VEC3_DOT(ptr_u, ptr_v) vec3_dot_product(ptr_u, ptr_v)
#define VEC3_ADD(u, v) vec3_add(u, v)
#define VEC3_SUB(u, v) vec3_sub(u, v)
#define VEC3_SCALE(k, u) vec3_scalar(k, u)
#define VEC3_MAG(ptr_u) vec3_magnitude(ptr_u)
#define VEC3_NORM(u) vec3_normalize(u)
#define QUAT_NORM(q) quaternion_normalize(q)
#define QUAT_CONJ(q) quaternion_conjugate(q)
#define QUAT_PROD(q1, q2) quaternion_product(q1, q2)
#define VEC3_ROT(u, axis, theta) vec3_rotate(u, axis, theta)

typedef struct cubVec3 {
    float coords[3];
} vec3;

typedef struct cubVec4 {
    float coords[4];
} vec4;

// Vector - Scalar representation of quaternions
typedef struct cubQuaternion {
    vec3 vect;
    float w;
} quaternion;

/* Return an initialised 3D vector (x, y, z) */
vec3 vec3_new(float x, float y, float z);
vec4 vec4_new(vec3 xyz, float w);
vec3 vec3_canonical_base(vec3 input, vec3 unit_x, vec3 unit_y, vec3 unit_z);
float vec3_dot_product(vec3* v1, vec3* v2);
vec3 vec3_cross_product(vec3 v1, vec3 v2);
vec3 vec3_add(vec3 v1, vec3 v2);
vec3 vec3_sub(vec3 v1, vec3 v2);
vec3 vec3_scalar(float scalar, vec3 v);
float vec3_magnitude(vec3* v);
vec3 vec3_normalize(vec3 v);
quaternion quaternion_normalize(quaternion q);
quaternion quaternion_conjugate(quaternion q);
quaternion quaternion_product(quaternion q1, quaternion q2);
/* Rotates the 3D vector 'to_rotate' by 'angle' RADIANS around the rotation
 * axis 'axis'. The 3D vector axis SHOULD BE NORMALIZED in most cases ! */
vec3 vec3_rotate(vec3 to_rotate, vec3 axis, float angle);

#endif
