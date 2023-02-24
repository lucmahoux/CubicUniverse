#include "fs.h"

#include <err.h>
#include <string.h>
#include <stdlib.h>

#define CUB_FS_INPUT_BUFFER 2048

static struct cubFileSystem fs = { .assets_path = NULL, .appdata_path = NULL };
static char input_buffer[CUB_FS_INPUT_BUFFER];

void setup_filesystem(int argc, char **argv) {
    char *appdata_path = NULL;
    char *assets_path = NULL;

    for (int i = 1; i < argc && (appdata_path == NULL || assets_path == NULL); ++i) {
        if (argv[i][0] != '-')
            continue;

        if (strcmp(argv[i] + 1, "appdata_path") == 0)
            appdata_path = argv[i + 1];
        else if (strcmp(argv[i] + 1, "assets_path") == 0)
            assets_path = argv[i + 1];
    }

    if (appdata_path == NULL)
        errx(EXIT_FAILURE, "setup_filesystem: required argument `-appdata_path` not found!");

    if (assets_path == NULL)
        errx(EXIT_FAILURE, "setup_filesystem: required argument `-assets_path` not found!");

    fs.appdata_path = strdup(appdata_path);
    fs.assets_path = strdup(assets_path);
}

FILE *get_asset_file(const char *asset_path, const char *modes) {
    static char buffer[CUB_FS_INPUT_BUFFER];
    snprintf(buffer, CUB_FS_INPUT_BUFFER, "%s/%s", fs.assets_path, asset_path);
    return fopen(buffer, modes);
}

FILE *get_appdata_file(const char *appdata_path, const char *modes) {
    static char buffer[CUB_FS_INPUT_BUFFER];
    snprintf(buffer, CUB_FS_INPUT_BUFFER, "%s/%s", fs.appdata_path, appdata_path);
    return fopen(buffer, modes);
}

FILE *get_shader_file(const char *shader_name, const char *modes) {
    snprintf(input_buffer, CUB_FS_INPUT_BUFFER, "shaders/%s", shader_name);
    return get_asset_file(input_buffer, modes);
}

FILE *get_texture_file(const char *texture_name, const char *modes) {
    snprintf(input_buffer, CUB_FS_INPUT_BUFFER, "textures/%s", texture_name);
    return get_asset_file(input_buffer, modes);
}

void destroy_filesystem(void) {
    free(fs.appdata_path);
    free(fs.assets_path);
}
