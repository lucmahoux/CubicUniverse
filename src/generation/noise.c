#include "noise.h"
#include "stb_image_write.h"

/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 */
static float interpolate(float a0, float a1, float w) {
    if (0.0f > w) return a0;
    if (1.0f < w) return a1;
    return (a1 - a0) * (3.0f - w * 2.0f) * w * w + a0;
}

static float default_interpolate(float a0, float a1, float w) {
    if (0.0f > w) return a0;
    if (1.0f < w) return a1;
    return (a1 - a0) * w + a0;
}

static float normalize(float a0, float a1, float w) {
    if (w <= a0) return a0;
    if (w >= a1) return a1;
    return (w - a0) / (a1 - a0);
}

/* Create pseudorandom direction vector
 */
static vec2 random_gradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; // rotation width
    unsigned a = ix;
    unsigned b = iy;

    a *= 3284157443;
    b ^= a << s | a >> (w - s);
    b *= 1911520717;
    a ^= b << s | b >> (w - s);
    a *= 2048419325;

    float random = (float) (a * (3.14159265 / ~(~0u >> 1))); // in [0, 2*Pi]
    vec2 gradient = {
            .x = cosf(random),
            .y = sinf(random)
    };

    return gradient;
}

// Computes the dot product of the distance and gradient vectors.
static float dot_grid_gradient(int ix, int iy, float x, float y) {
    // Get gradient from integer coordinates
    vec2 gradient = random_gradient(ix, iy);

    // Compute the distance vector
    float dx = x - (float) ix;
    float dy = y - (float) iy;

    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}

// Compute Perlin noise at coordinates x, y
static float perlin_noise(float x, float y) {
    // Determine grid cell coordinates
    int x0 = (int) floorf(x);
    int x1 = x0 + 1;
    int y0 = (int) floorf(y);
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - (float) x0;
    float sy = y - (float) y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dot_grid_gradient(x0, y0, x, y);
    n1 = dot_grid_gradient(x1, y0, x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dot_grid_gradient(x0, y1, x, y);
    n1 = dot_grid_gradient(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return value; // Will return in range -1 to 1. To make it in range 0 to 1, multiply by 0.5 and add 0.5
}

static void clamp_arguments_values(struct noise_generator_args *args) {
    if (args->scale <= 0)
        args->scale = 0.00001f;
    if (args->width < 1)
        args->width = 1;
    if (args->height < 1)
        args->height = 1;
    if (args->nb_octaves < 0)
        args->nb_octaves = 0;
    if (args->lacunarity < 1.0f)
        args->lacunarity = 1.0f;
    if (args->persistence < 0.0f)
        args->persistence = 0.0f;
    else if (args->persistence > 1.0f)
        args->persistence = 1.0f;
}

static void normalize_map(struct noise_map *noise_map, float min_height, float max_height) {
    size_t size = noise_map->width * noise_map->height;
    for (size_t i = 0; i < size; ++i)
        noise_map->map[i] = normalize(min_height, max_height, noise_map->map[i]);
}

static vec2 *generate_random_offsets(int32_t nb_octaves, unsigned int seed, vec2 offset) {
    vec2 *offsets = malloc(nb_octaves * sizeof(vec2));

    if (offsets == NULL)
        errx(EXIT_FAILURE, "generate_random_offsets: malloc(3) failed");

    for (int32_t i = 0; i < nb_octaves; ++i) {
        offsets[i].x = (float)rand_r(&seed) / (float)(RAND_MAX / 200000) - 100000 + offset.x;
        offsets[i].y = (float)rand_r(&seed) / (float)(RAND_MAX / 200000) - 100000 + offset.y;
    }

    return offsets;
}

struct noise_map *generate_noise_map(struct noise_generator_args *args) {
    struct noise_map *noise_map = malloc(sizeof(struct noise_map));

    if (noise_map == NULL)
        errx(EXIT_FAILURE, "generate_noise_map: malloc(3) failed");

    clamp_arguments_values(args);

    noise_map->map = malloc(args->width * args->height * sizeof(float));
    noise_map->width = args->width;
    noise_map->height = args->height;

    if (noise_map->map == NULL)
        errx(EXIT_FAILURE, "generate_noise_map: malloc(3) failed");

    vec2 *octave_offsets = generate_random_offsets(args->nb_octaves, args->seed, args->offset);

    float min_height = INFINITY;
    float max_height = -INFINITY;

    for (int32_t x = 0; x < args->width; ++x) {
        for (int32_t y = 0; y < args->height; ++y) {
            float amplitude = 1;
            float frequency = 1;
            float noise_height = 0;

            for (int32_t i = 0; i < args->nb_octaves; ++i) {
                float sample_x = (float) x / args->scale * frequency + octave_offsets[i].x;
                float sample_y = (float) y / args->scale * frequency + octave_offsets[i].y;

                float sample_noise = perlin_noise(sample_x, sample_y);
                noise_height += sample_noise * amplitude;

                amplitude *= args->persistence;
                frequency *= args->lacunarity;
            }

            noise_map->map[y * args->width + x] = noise_height;

            if (noise_height > max_height)
                max_height = noise_height;
            else if (noise_height < min_height)
                min_height = noise_height;
        }
    }

    free(octave_offsets);
    normalize_map(noise_map, min_height, max_height);

    return noise_map;
}

void draw_noise_map(struct noise_map *noise_map, const char *path) {
    int channel_nbr = 1;
    uint8_t *pixels = malloc(noise_map->width * noise_map->height * channel_nbr);

    if (pixels == NULL)
        errx(EXIT_FAILURE, "draw_noise_map: malloc(3) failed");

    size_t index = 0;

    for (int32_t y = 0; y < noise_map->height; ++y) {
        for (int32_t x = 0; x < noise_map->width; ++x) {
            float sample = noise_map->map[y * noise_map->width + x];
            uint8_t grayscale = (uint8_t) default_interpolate(0.0f, 255.0f, sample);
            pixels[index++] = grayscale;
        }
    }

    stbi_write_png(path, noise_map->width, noise_map->height,channel_nbr, pixels, channel_nbr * noise_map->width);
    free(pixels);
}

void free_noise_map(struct noise_map *noise_map) {
    free(noise_map->map);
    free(noise_map);
}
