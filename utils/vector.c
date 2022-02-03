#include "utils/vector.h"

cubVec3 cub_utils_vec3(float x, float y, float z) {
    cubVec3 result;
    result.coords[0] = x;
    result.coords[1] = y;
    result.coords[2] = z;
    return result;
}

float cub_utils_vec3_dot_product(cubVec3* v1, cubVec3* v2) {
    return  v1->coords[0] * v2->coords[0] +
            v1->coords[1] * v2->coords[1] +
            v1->coords[2] * v2->coords[2];
}

cubVec3 cub_utils_vec3_cross_product(cubVec3 v1, cubVec3 v2) {
    return cub_utils_vec3(  v1.coords[1] * v2.coords[2] -
                            v1.coords[2] * v2.coords[1],
                            v1.coords[2] * v2.coords[0] -
                            v1.coords[0] * v2.coords[2],
                            v1.coords[0] * v2.coords[1] -
                            v1.coords[1] * v2.coords[0] );
}

cubVec3 cub_utils_vec3_add(cubVec3 v1, cubVec3 v2) {
    v1.coords[0] += v2.coords[0];
    v1.coords[1] += v2.coords[1];
    v1.coords[2] += v2.coords[2];
    return v1;
}

cubVec3 cub_utils_vec3_sub(cubVec3 v1, cubVec3 v2) {
    v1.coords[0] -= v2.coords[0];
    v1.coords[1] -= v2.coords[1];
    v1.coords[2] -= v2.coords[2];
    return v1;
}

cubVec3 cub_utils_vec3_scalar(float scalar, cubVec3 v) {
    v.coords[0] *= scalar;
    v.coords[1] *= scalar;
    v.coords[2] *= scalar;
    return v;
}

float cub_utils_vec3_magnitude(cubVec3* v) {
    float sum = v->coords[0] * v->coords[0] +
                v->coords[1] * v->coords[1] +
                v->coords[2] * v->coords[2];
    return sqrt(sum);
}

cubVec3 cub_utils_vec3_normalize(cubVec3 v) {
    return CUB_VEC3_SCALE(1.0f / CUB_VEC3_MAG(&v), v);
}

cubQuaternion cub_utils_quaternion_normalize(cubQuaternion q) {
    float sum = q.vect.coords[0] * q.vect.coords[0] +
                q.vect.coords[1] * q.vect.coords[1] +
                q.vect.coords[2] * q.vect.coords[2] +
                q.w * q.w;
    sum = sqrt(sum);
    q.vect = CUB_VEC3_SCALE(1.0f / sum, q.vect);
    q.w /= sum;
    return q;
}

cubQuaternion cub_utils_quaternion_conjugate(cubQuaternion q) {
    q.vect = cub_utils_vec3_scalar(-1.0f, q.vect);
    return q;
}

cubQuaternion cub_utils_quaternion_product(cubQuaternion q1,
                                            cubQuaternion q2) {
    cubQuaternion result;
    result.w = q1.w * q2.w - CUB_VEC3_DOT(&q1.vect, &q2.vect);
    result.vect = CUB_VEC3_ADD( CUB_VEC3_SCALE(q1.w, q2.vect),
                                CUB_VEC3_SCALE(q2.w, q1.vect) );
    result.vect = CUB_VEC3_ADD( CUB_VEC3_CROSS(q1.vect, q2.vect), result.vect);
    return result;
}

cubVec3 cub_utils_vec3_rotate(cubVec3 to_rotate, cubVec3 axis, float angle) {
    cubQuaternion init = { .vect = to_rotate, .w = 0 };
    cubQuaternion axis_q;
    axis_q.vect = cub_utils_vec3_scalar(sin(angle / 2.0f), axis);
    axis_q.w = cos(angle / 2.0f);
    return CUB_QUAT_PROD(CUB_QUAT_PROD(axis_q, init),
                         CUB_QUAT_CONJ(axis_q)).vect;
}
