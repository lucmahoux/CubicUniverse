#ifndef CUB_UTILS_MATRIX_H
#define CUB_UTILS_MATRIX_H
#include "utils/utils.h"
#include "utils/vector.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD(x) x * M_PI / 180

typedef struct cub_matrix_4 {
    float coeffs[16];
} cub_mat4;

/* Prints a matrix */
void cub_utils_mat4_print(cub_mat4* matrix);

/* Create a 4x4 matrix & set the diagonal coefficients to 'diag_coeff'.
 * The other coefficients are set to 0 */
cub_mat4 cub_utils_mat4(float diag_coeff);

/* Returns the result of m1 x m2 */
cub_mat4 cub_utils_mat4_product(cub_mat4 m1, cub_mat4 m2);

/* Apply a translation transform to the matrix
 * (= last column set to 'translation' vector) */
cub_mat4 cub_utils_mat4_translate(cub_mat4 matrix, cub_vec3 translation);

/* Apply a scaling transform to the matrix (= diagonal coefficients
 * multiplied by the relevant coeffs in 'scaling' vector) */
cub_mat4 cub_utils_mat4_scale(cub_mat4 matrix, cub_vec3 scaling);

/* Apply a rotation transform to the matrix of 'angle' radians
 * around 'rotation_axis' which MUST be a NORMALIZED vector.
 * This should be the LAST transformation applied to 'matrix' before
 * transforming vectors since it returns the product of ['matrix' x 'rot_m']
 * where 'rot_m' is the rotation matrix */
cub_mat4 cub_utils_mat4_rotate(cub_mat4 matrix, float angle,
                                cub_vec3 rotation_axis);

/* Returns the perspective projection matrix (View space to Clip space) */
cub_mat4 cub_utils_mat4_perspective(float fov, float aspect_ratio,
                                    float near, float far);

/* Returns the view matrix (World space to View space) where
 *  - 'position' is the camera position in world coords
 *  - 'target' is the position where the camera must look at in world coords
 *  - 'up' is a NORMALIZED vector used to determine the orthonormal basis
 *     of the view matrix - usually (0, 1, 0) */
cub_mat4 cub_utils_mat4_view_matrix(cub_vec3 position, cub_vec3 target,
                                    cub_vec3 up);

#endif
