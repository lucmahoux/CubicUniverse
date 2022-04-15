#include <criterion/criterion.h>
#include "utils/matrix.h"

Test(Matrix, init_matrix) {
    mat4 M = mat4_new(4.0f);
    mat4 expected;
    memset(expected.coeffs, 0, sizeof(float) * 16);
    expected.coeffs[0] = 4.0f;
    expected.coeffs[5] = 4.0f;
    expected.coeffs[10] = 4.0f;
    expected.coeffs[15] = 4.0f;
    cr_assert_arr_eq(M.coeffs, expected.coeffs, sizeof(float) * 16);
}

Test(Matrix, translate_transformation) {
    mat4 M = mat4_new(1.0f);
    mat4 expected = mat4_new(1.0f);
    vec3 translation = vec3_new(1.0f, 2.0f, 3.0f);
    M = mat4_translate(M, translation);
    expected.coeffs[12] = 1.0f;
    expected.coeffs[13] = 2.0f;
    expected.coeffs[14] = 3.0f;
    cr_assert_arr_eq(M.coeffs, expected.coeffs, sizeof(float) * 16);
    translation = vec3_new(4.0f, 5.0f, 6.0f);
    M = mat4_translate(M, translation);
    expected.coeffs[12] = 5.0f;
    expected.coeffs[13] = 7.0f;
    expected.coeffs[14] = 9.0f;
    cr_assert_arr_eq(M.coeffs, expected.coeffs, sizeof(float) * 16);
}

Test(Matrix, scale_transformation) {
    mat4 M = mat4_new(1.0f);
    mat4 expected = mat4_new(1.0f);
    vec3 scaling = vec3_new(1.0f, 2.0f, 3.0f);
    M = mat4_scale(M, scaling);
    expected.coeffs[0] = 1.0f;
    expected.coeffs[5] = 2.0f;
    expected.coeffs[10] = 3.0f;
    cr_assert_arr_eq(M.coeffs, expected.coeffs, sizeof(float) * 16);
    scaling = vec3_new(4.0f, 5.0f, 6.0f);
    M = mat4_scale(M, scaling);
    expected.coeffs[0] = 4.0f;
    expected.coeffs[5] = 10.0f;
    expected.coeffs[10] = 18.0f;
    cr_assert_arr_eq(M.coeffs, expected.coeffs, sizeof(float) * 16);
}

Test(Matrix, product_matrix) {
    mat4 m1;
    m1.coeffs[0] = 1.0f;
    m1.coeffs[1] = 2.0f;
    m1.coeffs[2] = 3.0f;
    m1.coeffs[3] = -1.0f;
    m1.coeffs[4] = -4.0f;
    m1.coeffs[5] = 5.0f;
    m1.coeffs[6] = 10.0f;
    m1.coeffs[7] = -2.0f;
    m1.coeffs[8] = 2.0f;
    m1.coeffs[9] = 6.0f;
    m1.coeffs[10] = -7.0f;
    m1.coeffs[11] = 3.0f;
    m1.coeffs[12] = 1.0f;
    m1.coeffs[13] = 2.0f;
    m1.coeffs[14] = 3.0f;
    m1.coeffs[15] = 4.0f;
    mat4 m2;
    m2.coeffs[0] = -5.0f;
    m2.coeffs[1] = 4.0f;
    m2.coeffs[2] = 6.0f;
    m2.coeffs[3] = -4.0f;
    m2.coeffs[4] = -3.0f;
    m2.coeffs[5] = 4.0f;
    m2.coeffs[6] = 5.0f;
    m2.coeffs[7] = -8.0f;
    m2.coeffs[8] = 10.0f;
    m2.coeffs[9] = 6.0f;
    m2.coeffs[10] = -14.0f;
    m2.coeffs[11] = 6.0f;
    m2.coeffs[12] = 9.0f;
    m2.coeffs[13] = 3.0f;
    m2.coeffs[14] = 2.0f;
    m2.coeffs[15] = 7.0f;
    mat4 expected;
    expected.coeffs[0] = -13.0f;
    expected.coeffs[1] = 38.0f;
    expected.coeffs[2] = -29.0f;
    expected.coeffs[3] = -1.0f;
    expected.coeffs[4] = -17.0f;
    expected.coeffs[5] = 28.0f;
    expected.coeffs[6] = -28.0f;
    expected.coeffs[7] = -22.0f;
    expected.coeffs[8] = -36.0f;
    expected.coeffs[9] = -22.0f;
    expected.coeffs[10] = 206.0f;
    expected.coeffs[11] = -40.0f;
    expected.coeffs[12] = 8.0f;
    expected.coeffs[13] = 59.0f;
    expected.coeffs[14] = 64.0f;
    expected.coeffs[15] = 19.0f;
    mat4 result = mat4_product_simd(m1, m2);
    cr_assert_arr_eq(result.coeffs, expected.coeffs, sizeof(float) * 16);
    result = mat4_product_simd(m1, m2);
    cr_assert_arr_eq(result.coeffs, expected.coeffs, sizeof(float) * 16);
}

Test(Matrix, mat4_vec3_product) {
    mat4 m1;
    m1.coeffs[0] = 1.0f;
    m1.coeffs[1] = 2.0f;
    m1.coeffs[2] = 3.0f;
    m1.coeffs[3] = -1.0f;
    m1.coeffs[4] = -4.0f;
    m1.coeffs[5] = 5.0f;
    m1.coeffs[6] = 10.0f;
    m1.coeffs[7] = -2.0f;
    m1.coeffs[8] = 2.0f;
    m1.coeffs[9] = 6.0f;
    m1.coeffs[10] = -7.0f;
    m1.coeffs[11] = 3.0f;
    m1.coeffs[12] = 1.0f;
    m1.coeffs[13] = 2.0f;
    m1.coeffs[14] = 3.0f;
    m1.coeffs[15] = 4.0f;
    vec3 src = vec3_new(2.0f, -5.0f, 3.0f);
    vec4 result = mat4_vec3_product(m1, src);
    vec4 expected;
    expected.coords[0] = 29.0f;
    expected.coords[1] = -1.0f;
    expected.coords[2] = -62.0f;
    expected.coords[3] = 21.0f;
    cr_assert_arr_eq(result.coords, expected.coords, sizeof(float) * 4);
    src = vec3_new(-10.0f, -3.0f, 10.0f);
    result = mat4_vec3_product(m1, src);
    expected.coords[0] = 23.0f;
    expected.coords[1] = 27.0f;
    expected.coords[2] = -127.0f;
    expected.coords[3] = 50.0f;
    cr_assert_arr_eq(result.coords, expected.coords, sizeof(float) * 4);
}
