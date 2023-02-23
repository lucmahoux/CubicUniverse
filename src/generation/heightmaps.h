#ifndef CUB_GENERATION_HEIGHTMAPS_H
#define CUB_GENERATION_HEIGHTMAPS_H

#include "utils.h"
#include "types/chunk.h"

void fill_continentalness_heightmap(float heightmap[CHUNK_SLICE], unsigned int seed, vec2 chunk_position);

void fill_erosion_heightmap(float heightmap[CHUNK_SLICE], unsigned int seed, vec2 chunk_position);

void fill_peaks_and_valleys_heightmap(float heightmap[CHUNK_SLICE], unsigned int seed, vec2 chunk_position);

void generate_and_save_heightmaps(void);

#endif /* !CUB_GENERATION_HEIGHTMAPS_H */
