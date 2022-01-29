#ifndef CUB_UTILS_UTILS_H
#define CUB_UTILS_UTILS_H
#define cub_unused __attribute__((unused))

#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern char ASSETS_PATH[];
extern char SHADERS_PATH[];

/* Concatenate two strings and return the total length (excluding '\0') */
char* cub_utils_strconcat(const char* s1, const char* s2, size_t* len);

#endif
