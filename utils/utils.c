#include "utils/utils.h"

char ASSETS_PATH[] = "../assets/";
char SHADERS_PATH[] = "../assets/shaders/";
char TEXTURES_PATH[] = "../assets/textures/";

char* cub_utils_strconcat(const char* s1, const char* s2, size_t* len) {
    size_t s1_len = strlen(s1);
    size_t s2_len = strlen(s2);
    char* result = malloc((s1_len + s2_len + 1) * sizeof(char));
    if (!result)
        errx(1, "cub_utils_strconcat: malloc failed!");

    char* q = result;
    for (size_t i = 0; i < s1_len; ++i, ++q)
        *q = s1[i];
    for (size_t i = 0; i < s2_len; ++i, ++q)
        *q = s2[i];
    *q = '\0';
    if (len) *len = s1_len + s2_len;
    return result;
}
