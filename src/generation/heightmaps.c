#include "heightmaps.h"
#include "noise.h"

void fill_continentalness_heightmap(float heightmap[CHUNK_SLICE], unsigned int seed, vec2 chunk_position) {
    struct noise_generator_args args = {
            .persistence = 0.5f,
            .lacunarity = 2.0f,
            .nb_octaves = 4,
            .width = CHUNK_WIDTH,
            .height = CHUNK_DEPTH,
            .offset = chunk_position,
            .scale = 27.6f,
            .seed = seed
    };

    vec2 splines[] = {
            { .x = -1.0f, .y = 0.0f },
            { .x = 0.3f, .y = 0.4f },
            { .x = 0.4f, .y = 0.8f },
            { .x = 1.0f, .y = 1.0f }
    };

    struct noise_map *continentalness = generate_noise_map(&args, splines, 4);
    memcpy(heightmap, continentalness->map, CHUNK_SLICE * sizeof(float));
    free_noise_map(continentalness);
}

void fill_erosion_heightmap(float heightmap[CHUNK_SLICE], unsigned int seed, vec2 chunk_position) {
    struct noise_generator_args args = {
            .persistence = 0.1f,
            .lacunarity = 5.0f,
            .nb_octaves = 2,
            .width = CHUNK_WIDTH,
            .height = CHUNK_DEPTH,
            .offset = chunk_position,
            .scale = 75.0f,
            .seed = seed
    };

    vec2 splines[] = {
            { .x = -1.0f, .y = 0.0f },
            { .x = 1.0f, .y = 1.0f }
    };

    struct noise_map *erosion = generate_noise_map(&args, splines, 2);
    memcpy(heightmap, erosion->map, CHUNK_SLICE * sizeof(float));
    free_noise_map(erosion);
}

void fill_peaks_and_valleys_heightmap(float heightmap[CHUNK_SLICE], unsigned int seed, vec2 chunk_position) {
    struct noise_generator_args args = {
            .persistence = 0.05f,
            .lacunarity = 10.0f,
            .nb_octaves = 1,
            .width = CHUNK_WIDTH,
            .height = CHUNK_DEPTH,
            .offset = chunk_position,
            .scale = 25.0f,
            .seed = seed
    };

    vec2 splines[] = {
            { .x = -1.0f, .y = 0.0f },
            { .x = 1.0f, .y = 1.0f }
    };

    struct noise_map *temperature = generate_noise_map(&args, splines, 2);
    memcpy(heightmap, temperature->map, CHUNK_SLICE * sizeof(float));
    free_noise_map(temperature);
}

void generate_and_save_heightmaps(void) {
    struct noise_generator_args args = {
            .persistence = 0.5f,
            .lacunarity = 2.0f,
            .nb_octaves = 4,
            .width = CHUNK_WIDTH * 310,
            .height = CHUNK_DEPTH * 206,
            .offset = { .x = 0, .y = 0 },
            .scale = 900.0f,
            .seed = 125
    };

    vec2 splines[] = {
            { .x = -1.0f, .y = 1.0f },
            { .x = -0.8f,  .y = 0.05f },
            { .x = -0.35f, .y = 0.05f },
            { .x = -0.3f, .y = 0.45f },
            { .x = -0.05f, .y = 0.045f },
            { .x = -0.03f, .y = 0.85f },
            { .x = 0.0f, .y = 0.9f },
            { .x = 0.4f, .y = 0.97f },
            { .x= 1.0f, .y = 1.0f }
    };

    //struct noise_map *map = generate_noise_map(&args, splines, 9);
    //draw_noise_map(map, "continentalness.png");
    //free_noise_map(map);

    args.scale = 750.0f;
    args.persistence = 0.1f;
    args.lacunarity = 5.0f;
    args.nb_octaves = 2;

    vec2 splines2[] = {
            { .x = -1.0f, .y = 1.0f },
            { .x = -0.6f, .y = 0.55f },
            { .x = -0.25f, .y = 0.4f },
            { .x = -0.22f, .y = 0.45f },
            { .x = 0.0f, .y = 0.12f },
            { .x = 0.25f, .y = 0.1f },
            { .x = 0.45f, .y = 0.1f },
            { .x = 0.65f, .y = 0.1f },
            { .x = 0.675f, .y = 0.275f },
            { .x = 0.825f, .y = 0.275f },
            { .x = 0.85f, .y = 0.1f },
            { .x = 0.95f, .y = 0.05f },
            { .x = 1.0f, .y = 0.05f }
    };

    //map = generate_noise_map(&args, splines2, 13);
    //draw_noise_map(map, "erosion.png");
    //free_noise_map(map);

    args.scale = 175.0f;
    args.persistence = 0.2f;
    args.lacunarity = 4.0f;
    args.nb_octaves = 2;

    /*vec2 splines3[] = {
            { .x = -1.0f, .y = 0.0f },
            { .x = -0.6f, .y = 0.1f },
            { .x = -0.1f, .y = 0.2f },
            { .x = 0.2f, .y = 0.215f },
            { .x = 0.5f, .y = 0.55f },
            { .x = 0.7f, .y = 0.625f },
            { .x = 1.0f, .y = 0.6f }
    };*/

    vec2 splines3[] = {
            { .x = -1.0f, .y = 0.0f },
            { .x = -0.2f, .y = 0.0f },
            { .x = 0.0f, .y = 0.14f },
            { .x = 0.25f, .y = 0.29f },
            { .x = 0.4f, .y = 0.31f },
            { .x = 0.55f, .y = 0.76f },
            { .x = 0.65f, .y = 0.893f },
            { .x = 1.0f, .y = 1.0f }
    };

    struct noise_map *map = generate_noise_map(&args, splines3, 8);
    draw_noise_map(map, "peaks_and_valleys.png");
    free_noise_map(map);
}
