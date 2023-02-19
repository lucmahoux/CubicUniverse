#include "data_struct/vector.h"

vec3 vec3_new(float x, float y, float z) {
    vec3 result;
    result.coords[0] = x;
    result.coords[1] = y;
    result.coords[2] = z;
    return result;
}

vec4 vec4_new(vec3 xyz, float w) {
    vec4 result;
    result.coords[0] = xyz.coords[0];
    result.coords[1] = xyz.coords[1];
    result.coords[2] = xyz.coords[2];
    result.coords[3] = w;
    return result;
}

vec3 vec3_canonical_base(vec3 input, vec3 unit_x, vec3 unit_y, vec3 unit_z) {
    return VEC3_ADD(VEC3_ADD(VEC3_SCALE(input.coords[0], unit_x),
                             VEC3_SCALE(input.coords[1], unit_y)),
                    VEC3_SCALE(input.coords[2], unit_z));
}

float vec3_dot_product(vec3* v1, vec3* v2) {
    return  v1->coords[0] * v2->coords[0] +
            v1->coords[1] * v2->coords[1] +
            v1->coords[2] * v2->coords[2];
}

vec3 vec3_cross_product(vec3 v1, vec3 v2) {
    return vec3_new( v1.coords[1] * v2.coords[2] -
                            v1.coords[2] * v2.coords[1],
                            v1.coords[2] * v2.coords[0] -
                            v1.coords[0] * v2.coords[2],
                            v1.coords[0] * v2.coords[1] -
                            v1.coords[1] * v2.coords[0] );
}

vec3 vec3_add(vec3 v1, vec3 v2) {
    v1.coords[0] += v2.coords[0];
    v1.coords[1] += v2.coords[1];
    v1.coords[2] += v2.coords[2];
    return v1;
}

vec3 vec3_sub(vec3 v1, vec3 v2) {
    v1.coords[0] -= v2.coords[0];
    v1.coords[1] -= v2.coords[1];
    v1.coords[2] -= v2.coords[2];
    return v1;
}


vec3 vec3_scalar(float scalar, vec3 v) {
    v.coords[0] *= scalar;
    v.coords[1] *= scalar;
    v.coords[2] *= scalar;
    return v;
}

float vec3_magnitude(vec3* v) {
    float sum = v->coords[0] * v->coords[0] +
                v->coords[1] * v->coords[1] +
                v->coords[2] * v->coords[2];
    return sqrtf(sum);
}

vec3 vec3_normalize(vec3 v) {
    return VEC3_SCALE(1.0f / VEC3_MAG(&v), v);
}

quaternion quaternion_normalize(quaternion q) {
    float sum = q.vector.coords[0] * q.vector.coords[0] +
                q.vector.coords[1] * q.vector.coords[1] +
                q.vector.coords[2] * q.vector.coords[2] +
                q.w * q.w;
    sum = sqrtf(sum);
    q.vector = VEC3_SCALE(1.0f / sum, q.vector);
    q.w /= sum;
    return q;
}

quaternion quaternion_conjugate(quaternion q) {
    q.vector = vec3_scalar(-1.0f, q.vector);
    return q;
}

quaternion quaternion_product(quaternion q1,
                                            quaternion q2) {
    quaternion result;
    result.w = q1.w * q2.w - VEC3_DOT(&q1.vector, &q2.vector);
    result.vector = VEC3_ADD(VEC3_SCALE(q1.w, q2.vector),
                             VEC3_SCALE(q2.w, q1.vector) );
    result.vector = VEC3_ADD(VEC3_CROSS(q1.vector, q2.vector), result.vector);
    return result;
}

vec3 vec3_rotate(vec3 to_rotate, vec3 axis, float angle) {
    quaternion init = { .vector = to_rotate, .w = 0 };
    quaternion axis_q;
    axis_q.vector = vec3_scalar(sinf(angle / 2.0f), axis);
    axis_q.w = cosf(angle / 2.0f);
    return QUAT_PROD(QUAT_PROD(axis_q, init),
                         QUAT_CONJ(axis_q)).vector;
}
