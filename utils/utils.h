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
#include "matrix.h"
#include "vector.h"

extern char ASSETS_PATH[];
extern char SHADERS_PATH[];
extern char TEXTURES_PATH[];
extern char GAMEFILES_PATH[];

#define cub_block_t uint32_t

/* Concatenate two strings and return the total length (excluding '\0') */
char* cub_utils_strconcat(const char* s1, const char* s2, size_t* len);

#endif
