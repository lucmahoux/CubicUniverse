#ifndef CUB_UTILS_UTILS_H
#define CUB_UTILS_UTILS_H
#define cub_unused __attribute__((unused))

#include <err.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// OpenGL headers
#include <GL/glew.h> // -> Before (headers included from drivers)
#include <GLFW/glfw3.h>

// Linear Algebra
#include "data_struct/matrix.h"
#include "data_struct/vector.h"

#define CUB_TEX_ATLAS_WIDTH 512

/* Read memb_nbr elements of memb_size from file and put it in dest.
 * Errors are handled and exit the program! */
#define UTILS_FREAD(dest, memb_size, memb_nbr, fp, fname, field_name) { \
    if (fread(dest, memb_size, memb_nbr, fp) != memb_nbr)\
        errx(1, "%s: Wrong format on field '%s'!", fname, field_name);\
}

/* Write memb_nbr elements of memb_size to file from 'src'.
 * Errors are handled and exit the program! */
#define UTILS_FWRITE(src, memb_size, memb_nbr, fp, fname, field_name) {\
    if (fwrite(src, memb_size, memb_nbr, fp) != memb_nbr)\
        errx(1, "%s: fwrite() failed on field '%s'!", fname, field_name);\
}

/* Concatenate two strings and return the total length (excluding '\0') */
char* utils_strconcat(const char* s1, const char* s2, size_t* len);

void* utils_malloc(size_t size, const char* fname);

void* utils_realloc(void* src, size_t new_size, const char* fname);

#endif
