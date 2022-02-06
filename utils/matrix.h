#ifndef CUB_UTILS_MATRIX_H
#define CUB_UTILS_MATRIX_H

#include "utils/vector.h"

#define CUB_MAT4(coeff) cub_utils_mat4(coeff)
#define CUB_MAT4_PROD(m1, m2) cub_utils_mat4_product(m1, m2)
#define CUB_MAT4_TRANS(m, v) cub_utils_mat4_translate(m, v)
#define CUB_MAT4_SCALE(m, v) cub_utils_mat4_scale(m, v)
#define CUB_MAT4_ROT(m, theta, axis) cub_utils_mat4_rotate(m, theta, axis)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD(x) x * M_PI / 180

typedef struct cub_matrix_4 {
    float coeffs[16];
} cubMat4;

/* Prints a matrix */
void cub_utils_mat4_print(cubMat4* matrix);

/* Create a 4x4 matrix & set the diagonal coefficients to 'diag_coeff'.
 * The other coefficients are set to 0 */
cubMat4 cub_utils_mat4(float diag_coeff);

/* Returns the result of m1 x m2 */
cubMat4 cub_utils_mat4_product(cubMat4 m1, cubMat4 m2);

/* Apply a translation transform to the matrix
 * (= last column set to 'translation' vector) */
cubMat4 cub_utils_mat4_translate(cubMat4 matrix, cubVec3 translation);

/* Apply a scaling transform to the matrix (= diagonal coefficients
 * multiplied by the relevant coeffs in 'scaling' vector) */
cubMat4 cub_utils_mat4_scale(cubMat4 matrix, cubVec3 scaling);

/* Apply a rotation transform to the matrix of 'angle' radians
 * around 'rotation_axis' which MUST be a NORMALIZED vector.
 * This should be the LAST transformation applied to 'matrix' before
 * transforming vectors since it returns the product of ['matrix' x 'rot_m']
 * where 'rot_m' is the rotation matrix */
cubMat4 cub_utils_mat4_rotate(cubMat4 matrix, float angle,
                                cubVec3 rotation_axis);

/* Returns the perspective projection matrix (View space to Clip space) */
cubMat4 cub_utils_mat4_perspective(float fov, float aspect_ratio,
                                    float near, float far);

/* Returns the view matrix (World space to View space) where
 *  - 'position' is the camera position in world coords
 *  - 'target' is the position where the camera must look at in world coords
 *  - 'up' is a NORMALIZED vector used to determine the orthonormal basis
 *     of the view matrix - usually (0, 1, 0) */
cubMat4 cub_utils_mat4_view_matrix(cubVec3 position, cubVec3 target,
                                    cubVec3 up);

#endif
