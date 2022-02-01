#include "utils/vector.h"

cub_vec3 cub_utils_vec3(float x, float y, float z) {
    cub_vec3 result;
    result.coords[0] = x;
    result.coords[1] = y;
    result.coords[2] = z;
    return result;
}

float cub_utils_vec3_dot_product(cub_vec3* v1, cub_vec3* v2) {
    return  v1->coords[0] * v2->coords[0] +
            v1->coords[1] * v2->coords[1] +
            v1->coords[2] * v2->coords[2];
}

cub_vec3 cub_utils_vec3_cross_product(cub_vec3 v1, cub_vec3 v2) {
    return cub_utils_vec3(  v1.coords[1] * v2.coords[2] -
                            v1.coords[2] * v2.coords[1],
                            v1.coords[2] * v2.coords[0] -
                            v1.coords[0] * v2.coords[2],
                            v1.coords[0] * v2.coords[1] -
                            v1.coords[1] * v2.coords[0] );
}

cub_vec3 cub_utils_vec3_add(cub_vec3 v1, cub_vec3 v2) {
    v1.coords[0] += v2.coords[0];
    v1.coords[1] += v2.coords[1];
    v1.coords[2] += v2.coords[2];
    return v1;
}

cub_vec3 cub_utils_vec3_sub(cub_vec3 v1, cub_vec3 v2) {
    v1.coords[0] -= v2.coords[0];
    v1.coords[1] -= v2.coords[1];
    v1.coords[2] -= v2.coords[2];
    return v1;
}

cub_vec3 cub_utils_vec3_scalar(float scalar, cub_vec3 v) {
    v.coords[0] *= scalar;
    v.coords[1] *= scalar;
    v.coords[2] *= scalar;
    return v;
}

float cub_utils_vec3_magnitude(cub_vec3* v) {
    float sum = v->coords[0] * v->coords[0] +
                v->coords[1] * v->coords[1] +
                v->coords[2] * v->coords[2];
    return sqrt(sum);
}

cub_vec3 cub_utils_vec3_normalize(cub_vec3 v) {
    return cub_utils_vec3_scalar(1.0f / cub_utils_vec3_magnitude(&v), v);
}

cub_quaternion cub_utils_quaternion_normalize(cub_quaternion q) {
    float sum = q.vect.coords[0] * q.vect.coords[0] +
                q.vect.coords[1] * q.vect.coords[1] +
                q.vect.coords[2] * q.vect.coords[2] +
                q.w * q.w;
    sum = sqrt(sum);
    q.vect = cub_utils_vec3_scalar(1.0f / sum, q.vect);
    q.w /= sum;
    return q;
}

cub_quaternion cub_utils_quaternion_conjugate(cub_quaternion q) {
    q.vect = cub_utils_vec3_scalar(-1.0f, q.vect);
    return q;
}

cub_quaternion cub_utils_quaternion_product(cub_quaternion q1,
                                            cub_quaternion q2) {
    cub_quaternion result;
    result.w = q1.w * q2.w - cub_utils_vec3_dot_product(&q1.vect, &q2.vect);
    result.vect = cub_utils_vec3_add(cub_utils_vec3_scalar(q1.w, q2.vect),
                                     cub_utils_vec3_scalar(q2.w, q1.vect));
    result.vect = cub_utils_vec3_add(
                    cub_utils_vec3_cross_product(q1.vect, q2.vect),
                    result.vect);
    return result;
}

cub_vec3 cub_utils_vec3_rotate(cub_vec3 to_rotate, cub_vec3 axis, float angle) {
    cub_quaternion init = { .vect = to_rotate, .w = 0 };
    cub_quaternion axis_q;
    axis_q.vect = cub_utils_vec3_scalar(sin(angle / 2.0f), axis);
    axis_q.w = cos(angle / 2.0f);
    return cub_utils_quaternion_product(
                cub_utils_quaternion_product(axis_q, init),
                cub_utils_quaternion_conjugate(axis_q)).vect;
}
