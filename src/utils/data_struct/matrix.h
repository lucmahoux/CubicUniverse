#ifndef CUB_UTILS_MATRIX_H
#define CUB_UTILS_MATRIX_H

// SSE SIMD intrinsics
#include <xmmintrin.h>
#include "data_struct/vector.h"

#define MAT4(coefficient) mat4_new(coefficient)
#define MAT4_PROD(m1, m2) mat4_product_simd(m1, m2)
#define MAT4_TRANS(m, v) mat4_translate(m, v)
#define MAT4_SCALE(m, v) mat4_scale(m, v)
#define MAT4_ROT(m, theta, axis) mat4_rotate(m, theta, axis)

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define RAD(x) (x * M_PI / 180)

typedef struct cubMatrix4 {
    float coefficients[16];
} mat4;

/* Prints a matrix */
void mat4_print(mat4* matrix);

/* Create a 4x4 matrix & set the diagonal coefficients to 'diag_coefficient'.
 * The other coefficients are set to 0 */
mat4 mat4_new(float diag_coefficient);

/* Returns the result of "m * v" where the w coordinate of 'v' is set to 1 */
vec4 mat4_vec3_product(mat4 m, vec3 v);

/* Matrix product using SIMD -> faster */
mat4 mat4_product_simd(mat4 m1, mat4 m2);

/* Apply a translation transform to the matrix */
mat4 mat4_translate(mat4 matrix, vec3 translation);

/* Apply a scaling transform to the matrix (= diagonal coefficients
 * multiplied by the relevant coefficients in 'scaling' vector) */
mat4 mat4_scale(mat4 matrix, vec3 scaling);

/* Apply a rotation transform to the matrix of 'angle' radians
 * about 'rotation_axis' which MUST be a NORMALIZED vector.
 * This should be the LAST transformation applied to 'matrix' before
 * transforming vectors since it returns the product of ['matrix' x 'rot_m']
 * where 'rot_m' is the rotation matrix */
mat4 mat4_rotate(mat4 matrix, float angle, vec3 rotation_axis);

/* Returns the perspective projection matrix (View space to Clip space) */
mat4 mat4_perspective(float fov, float aspect_ratio, float near, float far);

/* Returns the view matrix (World space to View space) where
 *  - 'position' is the camera position in world coords
 *  - 'target' is the position where the camera must look at in world coords
 *  - 'up' is a NORMALIZED vector used to determine the orthonormal basis
 *     of the view matrix - usually (0, 1, 0) */
mat4 mat4_view_matrix(vec3 position, vec3 target, vec3 up);

#endif
