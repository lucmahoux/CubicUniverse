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

extern char ASSETS_PATH[];
extern char SHADERS_PATH[];
extern char TEXTURES_PATH[];
extern char GAMEFILES_PATH[];

/* Concatenate two strings and return the total length (excluding '\0') */
char* utils_strconcat(const char* s1, const char* s2, size_t* len);

/* Returns TRUE if the file 'fname' exists */
bool utils_file_exists(const char* fname);

/* Read memb_nbr elements of memb_size from file and put it in dest.
 * Errors are handled and exit the program! */
void utils_fread(void* dest, size_t memb_size, size_t memb_nbr,
                 FILE* fp, const char* fname, const char* field_name);

/* Write memb_nbr elements of memb_size to file from 'src'.
 * Errors are handled and exit the program! */
void utils_fwrite(void* src, size_t memb_size, size_t memb_nbr,
                  FILE* fp, const char* fname, const char* field_name);

void* utils_malloc(size_t size, const char* fname);

void* utils_realloc(void* src, size_t new_size, const char* fname);

#endif
