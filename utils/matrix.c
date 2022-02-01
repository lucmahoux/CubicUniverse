#include "utils/matrix.h"

void cub_utils_mat4_print(cub_mat4* matrix) {
    for (uint8_t i = 0; i < 4; ++i) {
        for (uint8_t j = 0; j < 4; ++j)
            printf(" %f", matrix->coeffs[4 * j + i]);
        printf("\n");
    }
    printf("\n");
}

cub_mat4 cub_utils_mat4(float diag_coeff) {
    cub_mat4 matrix;
    for (uint8_t i = 0; i < 16; ++i)
        matrix.coeffs[i] = i % 5 == 0 ? diag_coeff : 0.0f;
    return matrix;
}

cub_mat4 cub_utils_mat4_product(cub_mat4 m1, cub_mat4 m2) {
    cub_mat4 result;
    for (uint8_t i = 0; i < 4; ++i) {
        for (uint8_t j = 0; j < 4; ++j) {
            float sum = 0;
            for (uint8_t k = 0; k < 4; ++k)
                sum += m1.coeffs[4 * k + i] * m2.coeffs[4 * j + k];
            result.coeffs[4 * j + i] = sum;
        }
    }
    return result;
}

cub_mat4 cub_utils_mat4_translate(cub_mat4 matrix, cub_vec3 translation) {
    matrix.coeffs[12] = translation.coords[0];
    matrix.coeffs[13] = translation.coords[1];
    matrix.coeffs[14] = translation.coords[2];
    return matrix;
}

cub_mat4 cub_utils_mat4_scale(cub_mat4 matrix, cub_vec3 scaling) {
    matrix.coeffs[0] *= scaling.coords[0];
    matrix.coeffs[5] *= scaling.coords[1];
    matrix.coeffs[10] *= scaling.coords[2];
    return matrix;
}

cub_mat4 cub_utils_mat4_rotate(cub_mat4 matrix, float angle,
                                cub_vec3 rotation_axis) {
    float cos_t = cos(angle), sin_t = sin(angle);
    float cos_1_t = 1 - cos_t;
    float rx = rotation_axis.coords[0],
            ry = rotation_axis.coords[1],
            rz = rotation_axis.coords[2];

    cub_mat4 rot_m = cub_utils_mat4(1.0f);
    rot_m.coeffs[0] = cos_t + rx * rx * cos_1_t;
    rot_m.coeffs[1] = rx * ry * cos_1_t + rz * sin_t;
    rot_m.coeffs[2] = rx * rz * cos_1_t - ry * sin_t;
    rot_m.coeffs[4] = rx * ry * cos_1_t - rz * sin_t;
    rot_m.coeffs[5] = cos_t + ry * ry * cos_1_t;
    rot_m.coeffs[6] = ry * rz * cos_1_t + rx * sin_t;
    rot_m.coeffs[8] = rx * rz * cos_1_t + ry * sin_t;
    rot_m.coeffs[9] = ry * rz * cos_1_t - rx * sin_t;
    rot_m.coeffs[10] = cos_t + rz * rz * cos_1_t;

    return cub_utils_mat4_product(matrix, rot_m);
}

cub_mat4 cub_utils_mat4_perspective(float fov, float aspect_ratio,
                                    float near, float far) {
    float width = 2.0f * near * tan(fov / 2.0f);
    float height = width / aspect_ratio;
    cub_mat4 result = cub_utils_mat4(1.0f);
    result.coeffs[0] = 2.0f * near / width;
    result.coeffs[5] = 2.0f * near / height;
    result.coeffs[10] = -(far + near) / (far - near);
    result.coeffs[11] = -1;
    result.coeffs[14] = -2.0f * far * near / (far - near);
    return result;
}

cub_mat4 cub_utils_mat4_view_matrix(cub_vec3 position, cub_vec3 target,
                                    cub_vec3 up) {
    cub_mat4 view = cub_utils_mat4(1.0f);
    cub_vec3 direction = CUB_VEC3_NORM(CUB_VEC3_SUB(position, target));
    cub_vec3 right = CUB_VEC3_NORM(CUB_VEC3_CROSS(up, direction));
    up = cub_utils_vec3_cross_product(direction, right);
    view.coeffs[0] = right.coords[0];
    view.coeffs[1] = up.coords[0];
    view.coeffs[2] = direction.coords[0];
    view.coeffs[4] = right.coords[1];
    view.coeffs[5] = up.coords[1];
    view.coeffs[6] = direction.coords[1];
    view.coeffs[8] = right.coords[2];
    view.coeffs[9] = up.coords[2];
    view.coeffs[10] = direction.coords[2];
    cub_mat4 cam_position = CUB_MAT4_TRANS( CUB_MAT4(1.0f),
                                            CUB_VEC3_SCALE(-1.0f, position) );
    return cub_utils_mat4_product(view, cam_position);
}
