#include "data_struct/matrix.h"

void mat4_print(mat4* matrix) {
    for (uint8_t i = 0; i < 4; ++i) {
        for (uint8_t j = 0; j < 4; ++j)
            printf(" %f", matrix->coefficients[4 * j + i]);
        printf("\n");
    }
    printf("\n");
}

mat4 mat4_new(float diag_coefficient) {
    mat4 matrix;
    for (uint8_t i = 0; i < 16; ++i)
        matrix.coefficients[i] = i % 5 == 0 ? diag_coefficient : 0.0f;
    return matrix;
}

mat4 mat4_product_simd(mat4 m1, mat4 m2) {
    __m128 columns[4] = {
        _mm_setzero_ps(),
        _mm_setzero_ps(),
        _mm_setzero_ps(),
        _mm_setzero_ps()
    };
    for (uint8_t i = 0; i < 4; ++i) {
        __m128 m1_col = _mm_load_ps(m1.coefficients + (i << 2));
        for (uint8_t j = 0; j < 4; ++j) {
            __m128 coefficient = _mm_set1_ps(m2.coefficients[(j << 2) + i]);
            columns[j] = _mm_add_ps(columns[j], _mm_mul_ps(m1_col, coefficient));
        }
    }
    mat4 result;
    for (uint8_t i = 0; i < 4; ++i)
        _mm_store_ps(result.coefficients + (i << 2), columns[i]);
    return result;
}

vec4 mat4_vec3_product(mat4 m, vec3 v) {
    __m128 total = _mm_setzero_ps();
    for (uint8_t i = 0; i < 3; ++i) {
        __m128 column = _mm_load_ps(m.coefficients + (i << 2));
        __m128 coefficient = _mm_set1_ps(v.coords[i]);
        total = _mm_add_ps(total, _mm_mul_ps(column, coefficient));
    }
    total = _mm_add_ps(total, _mm_load_ps(m.coefficients + 12));
    vec4 result;
    _mm_store_ps(result.coords, total);
    return result;
}

mat4 mat4_translate(mat4 matrix, vec3 translation) {
    mat4 trans = MAT4(1.0f);
    trans.coefficients[12] = translation.coords[0];
    trans.coefficients[13] = translation.coords[1];
    trans.coefficients[14] = translation.coords[2];
    return mat4_product_simd(matrix, trans);
}

mat4 mat4_scale(mat4 matrix, vec3 scaling) {
    matrix.coefficients[0] *= scaling.coords[0];
    matrix.coefficients[5] *= scaling.coords[1];
    matrix.coefficients[10] *= scaling.coords[2];
    return matrix;
}

mat4 mat4_rotate(mat4 matrix, float angle, vec3 rotation_axis) {
    float cos_t = cosf(angle), sin_t = sinf(angle);
    float cos_1_t = 1 - cos_t;
    float rx = rotation_axis.coords[0],
            ry = rotation_axis.coords[1],
            rz = rotation_axis.coords[2];

    mat4 rot_m = mat4_new(1.0f);
    rot_m.coefficients[0] = cos_t + rx * rx * cos_1_t;
    rot_m.coefficients[1] = rx * ry * cos_1_t + rz * sin_t;
    rot_m.coefficients[2] = rx * rz * cos_1_t - ry * sin_t;
    rot_m.coefficients[4] = rx * ry * cos_1_t - rz * sin_t;
    rot_m.coefficients[5] = cos_t + ry * ry * cos_1_t;
    rot_m.coefficients[6] = ry * rz * cos_1_t + rx * sin_t;
    rot_m.coefficients[8] = rx * rz * cos_1_t + ry * sin_t;
    rot_m.coefficients[9] = ry * rz * cos_1_t - rx * sin_t;
    rot_m.coefficients[10] = cos_t + rz * rz * cos_1_t;

    return mat4_product_simd(matrix, rot_m);
}

mat4 mat4_perspective(float fov, float aspect_ratio, float near, float far) {
    float height = 2.0f * near * tanf(fov / 2.0f);
    float width = height * aspect_ratio;
    mat4 result = mat4_new(1.0f);
    result.coefficients[0] = 2.0f * near / width;
    result.coefficients[5] = 2.0f * near / height;
    result.coefficients[10] = -(far + near) / (far - near);
    result.coefficients[11] = -1;
    result.coefficients[14] = -2.0f * far * near / (far - near);
    return result;
}

mat4 mat4_view_matrix(vec3 position, vec3 target, vec3 up) {
    mat4 view = mat4_new(1.0f);
    vec3 direction = VEC3_NORM(VEC3_SUB(position, target));
    vec3 right = VEC3_NORM(VEC3_CROSS(up, direction));
    up = VEC3_NORM(VEC3_CROSS(direction, right));
    view.coefficients[0] = right.coords[0];
    view.coefficients[1] = up.coords[0];
    view.coefficients[2] = direction.coords[0];
    view.coefficients[4] = right.coords[1];
    view.coefficients[5] = up.coords[1];
    view.coefficients[6] = direction.coords[1];
    view.coefficients[8] = right.coords[2];
    view.coefficients[9] = up.coords[2];
    view.coefficients[10] = direction.coords[2];
    mat4 cam_position = MAT4_TRANS(MAT4(1.0f), VEC3_SCALE(-1.0f, position));
    return mat4_product_simd(view, cam_position);
}
