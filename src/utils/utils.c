#include "utils.h"

char ASSETS_PATH[] = "../assets/";
char SHADERS_PATH[] = "../assets/shaders/";
char TEXTURES_PATH[] = "../assets/textures/";
char GAMEFILES_PATH[] = "../assets/gamefiles/";

char* utils_strconcat(const char* s1, const char* s2, size_t* len) {
    size_t s1_len = strlen(s1);
    size_t s2_len = strlen(s2);
    char* result = malloc((s1_len + s2_len + 1) * sizeof(char));
    if (!result)
        errx(1, "utils_strconcat: malloc failed!");

    char* q = result;
    for (size_t i = 0; i < s1_len; ++i, ++q)
        *q = s1[i];
    for (size_t i = 0; i < s2_len; ++i, ++q)
        *q = s2[i];
    *q = '\0';
    if (len) *len = s1_len + s2_len;
    return result;
}

void utils_fread(void* dest, size_t memb_size, size_t memb_nbr,
                 FILE* fp, const char* fname, const char* field_name) {
    if (fread(dest, memb_size, memb_nbr, fp) != memb_nbr)
        errx(1, "%s: Wrong format on field '%s'!", fname, field_name);
}

void utils_fwrite(void* src, size_t memb_size, size_t memb_nbr,
                  FILE* fp, const char* fname, const char* field_name) {
    if (fwrite(src, memb_size, memb_nbr, fp) != memb_nbr)
        errx(1, "%s: fwrite() failed on field '%s'!", fname, field_name);
}

bool utils_file_exists(const char* fname) {
    return access(fname, F_OK) == 0;
}

void* utils_malloc(size_t size, const char* fname) {
    void* result = malloc(size);
    if (!result)
        errx(EXIT_FAILURE, "%s: malloc() allocation failed!", fname);
    return result;
}

void* utils_realloc(void* src, size_t new_size, const char* fname) {
    void* result = realloc(src, new_size);
    if (!result)
        errx(EXIT_FAILURE, "%s:realloc() allocation failed!", fname);
    return result;
}
