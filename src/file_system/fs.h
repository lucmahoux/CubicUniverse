#ifndef CUB_FS_H
#define CUB_FS_H

#include <stdio.h>

struct cubFileSystem {
    char *assets_path;
    char *appdata_path;
};

void setup_filesystem(int argc, char **argv);

FILE *get_asset_file(const char *asset_path, const char *modes);

FILE *get_shader_file(const char *shader_name, const char *modes);

FILE *get_texture_file(const char *texture_name, const char *modes);

FILE *get_appdata_file(const char *appdata_path, const char *modes);

void destroy_filesystem(void);

#endif /* !CUB_FS_H */
