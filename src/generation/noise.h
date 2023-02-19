#ifndef CUB_GENERATION_NOISE_H
#define CUB_GENERATION_NOISE_H

#include "stb_image.h"
#include "utils.h"

struct noise_map {
    float *map;
    int32_t width;
    int32_t height;
};

struct noise_generator_args {
    int32_t width;
    int32_t height;
    float scale;
    float persistence;
    float lacunarity;
    vec2 offset;
    int32_t nb_octaves;
    unsigned int seed;
};

struct noise_map *generate_noise_map(struct noise_generator_args *args);

void draw_noise_map(struct noise_map *noise_map, const char *path);

void free_noise_map(struct noise_map *noise_map);

#endif /* !CUB_GENERATION_NOISE_H */
