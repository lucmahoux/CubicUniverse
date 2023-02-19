#include <criterion/criterion.h>
#include "data_struct/matrix.h"

Test(Matrix, init_matrix) {
    mat4 M = mat4_new(4.0f);
    mat4 expected;
    memset(expected.coefficients, 0, sizeof(float) * 16);
    expected.coefficients[0] = 4.0f;
    expected.coefficients[5] = 4.0f;
    expected.coefficients[10] = 4.0f;
    expected.coefficients[15] = 4.0f;
    cr_assert_arr_eq(M.coefficients, expected.coefficients, sizeof(float) * 16);
}

Test(Matrix, translate_transformation) {
    mat4 M = mat4_new(1.0f);
    mat4 expected = mat4_new(1.0f);
    vec3 translation = vec3_new(1.0f, 2.0f, 3.0f);
    M = mat4_translate(M, translation);
    expected.coefficients[12] = 1.0f;
    expected.coefficients[13] = 2.0f;
    expected.coefficients[14] = 3.0f;
    cr_assert_arr_eq(M.coefficients, expected.coefficients, sizeof(float) * 16);
    translation = vec3_new(4.0f, 5.0f, 6.0f);
    M = mat4_translate(M, translation);
    expected.coefficients[12] = 5.0f;
    expected.coefficients[13] = 7.0f;
    expected.coefficients[14] = 9.0f;
    cr_assert_arr_eq(M.coefficients, expected.coefficients, sizeof(float) * 16);
}

Test(Matrix, scale_transformation) {
    mat4 M = mat4_new(1.0f);
    mat4 expected = mat4_new(1.0f);
    vec3 scaling = vec3_new(1.0f, 2.0f, 3.0f);
    M = mat4_scale(M, scaling);
    expected.coefficients[0] = 1.0f;
    expected.coefficients[5] = 2.0f;
    expected.coefficients[10] = 3.0f;
    cr_assert_arr_eq(M.coefficients, expected.coefficients, sizeof(float) * 16);
    scaling = vec3_new(4.0f, 5.0f, 6.0f);
    M = mat4_scale(M, scaling);
    expected.coefficients[0] = 4.0f;
    expected.coefficients[5] = 10.0f;
    expected.coefficients[10] = 18.0f;
    cr_assert_arr_eq(M.coefficients, expected.coefficients, sizeof(float) * 16);
}

Test(Matrix, product_matrix) {
    mat4 m1;
    m1.coefficients[0] = 1.0f;
    m1.coefficients[1] = 2.0f;
    m1.coefficients[2] = 3.0f;
    m1.coefficients[3] = -1.0f;
    m1.coefficients[4] = -4.0f;
    m1.coefficients[5] = 5.0f;
    m1.coefficients[6] = 10.0f;
    m1.coefficients[7] = -2.0f;
    m1.coefficients[8] = 2.0f;
    m1.coefficients[9] = 6.0f;
    m1.coefficients[10] = -7.0f;
    m1.coefficients[11] = 3.0f;
    m1.coefficients[12] = 1.0f;
    m1.coefficients[13] = 2.0f;
    m1.coefficients[14] = 3.0f;
    m1.coefficients[15] = 4.0f;
    mat4 m2;
    m2.coefficients[0] = -5.0f;
    m2.coefficients[1] = 4.0f;
    m2.coefficients[2] = 6.0f;
    m2.coefficients[3] = -4.0f;
    m2.coefficients[4] = -3.0f;
    m2.coefficients[5] = 4.0f;
    m2.coefficients[6] = 5.0f;
    m2.coefficients[7] = -8.0f;
    m2.coefficients[8] = 10.0f;
    m2.coefficients[9] = 6.0f;
    m2.coefficients[10] = -14.0f;
    m2.coefficients[11] = 6.0f;
    m2.coefficients[12] = 9.0f;
    m2.coefficients[13] = 3.0f;
    m2.coefficients[14] = 2.0f;
    m2.coefficients[15] = 7.0f;
    mat4 expected;
    expected.coefficients[0] = -13.0f;
    expected.coefficients[1] = 38.0f;
    expected.coefficients[2] = -29.0f;
    expected.coefficients[3] = -1.0f;
    expected.coefficients[4] = -17.0f;
    expected.coefficients[5] = 28.0f;
    expected.coefficients[6] = -28.0f;
    expected.coefficients[7] = -22.0f;
    expected.coefficients[8] = -36.0f;
    expected.coefficients[9] = -22.0f;
    expected.coefficients[10] = 206.0f;
    expected.coefficients[11] = -40.0f;
    expected.coefficients[12] = 8.0f;
    expected.coefficients[13] = 59.0f;
    expected.coefficients[14] = 64.0f;
    expected.coefficients[15] = 19.0f;
    mat4 result = mat4_product_simd(m1, m2);
    cr_assert_arr_eq(result.coefficients, expected.coefficients, sizeof(float) * 16);
    result = mat4_product_simd(m1, m2);
    cr_assert_arr_eq(result.coefficients, expected.coefficients, sizeof(float) * 16);
}

Test(Matrix, mat4_vec3_product) {
    mat4 m1;
    m1.coefficients[0] = 1.0f;
    m1.coefficients[1] = 2.0f;
    m1.coefficients[2] = 3.0f;
    m1.coefficients[3] = -1.0f;
    m1.coefficients[4] = -4.0f;
    m1.coefficients[5] = 5.0f;
    m1.coefficients[6] = 10.0f;
    m1.coefficients[7] = -2.0f;
    m1.coefficients[8] = 2.0f;
    m1.coefficients[9] = 6.0f;
    m1.coefficients[10] = -7.0f;
    m1.coefficients[11] = 3.0f;
    m1.coefficients[12] = 1.0f;
    m1.coefficients[13] = 2.0f;
    m1.coefficients[14] = 3.0f;
    m1.coefficients[15] = 4.0f;
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
