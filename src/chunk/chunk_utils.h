#ifndef CUB_CHUNK_CHUNK_UTILS_H
#define CUB_CHUNK_CHUNK_UTILS_H

#include "block_utils.h"
#include "types/chunk.h"

/* Convert the position of a block in CHUNK coordinates to its position in
 * SUBCHUNK coordinates */
SubChunkCoords chunk_coords_to_subchunk(ChunkCoords c_coords);

/* Convert the position of a block in SUBCHUNK coordinates to its position in
 * CHUNK coordinates */
ChunkCoords subchunk_coords_to_chunk(SubChunk* sc, SubChunkCoords sc_coords);

/* Convert the position of a block in CHUNK coordinates to its position in
 * WORLD coordinates */
vec3 chunk_coords_to_world(Chunk* chunk, ChunkCoords c_coords);

/* Convert the position of a block in SUBCHUNK coordinates to its position in
 * WORLD coordinates */
vec3 subchunk_coords_to_world(Chunk* chunk, SubChunk* sc,
                              SubChunkCoords sc_coords);

/* Convert the position of a block in WORLD coordinates to its position in
 * CHUNK coordinates
 * [!] The macro WORLD_COORDS_TO_BLOCK must be used if the coordinates passed
 * as parameter are NOT explicits (floating numbers) [!] */
ChunkCoords world_coords_to_chunk(vec3 w_coords);

/* Initialize an empty chunk with no subchunks */
void chunk_init(Chunk* chunk, int32_t chunkX, int32_t chunkZ);

/* Returns the BP_elt of the subchunk 'SC' associated with the block at
 * location 'position' inside 'SC'.
 * Returns NULL if the block is AIR. */
BP_elt* subchunk_get_BP_elt(SubChunk* SC, SubChunkCoords position);

void subchunk_build_visibility_graph(SubChunk* sc, GraphMarker* block_marker);

/* Free the specified subchunk */
void subchunk_free(SubChunk* sc);

/* Free the specified chunk */
void chunk_free(Chunk* chunk);

#endif
