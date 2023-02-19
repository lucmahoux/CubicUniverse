#ifndef CUB_CHUNK_CHUNK_RENDERER_H
#define CUB_CHUNK_CHUNK_RENDERER_H

#include "chunk_loader.h"
#include "chunk_utils.h"
#include "chunk.h"
#include "utils.h"

/* Allocate all the memory required by the chunk renderer.
 * Should be called only once, at the start of the game.
 * It initializes the grid of chunk using chunk_renderer_init() function */
void chunk_renderer_setup(ChunkRenderer* renderer, BlockList* block_list,
                          vec3 position, uint8_t render_distance);

/* Should be called each frame to update the grid of chunk of the renderer.
 * It loads & unloads all the needed/useless chunks. */
void chunk_renderer_update(ChunkRenderer* renderer, BlockList* block_list,
                           vec3 position);

/* Should be called each frame to render the world */
void chunk_renderer_render(ChunkRenderer* renderer,
                           BlockRenderer* block_renderer,
                           Camera* camera);

/* Free the chunk renderer as well as all the loaded chunks inside its grid */
void chunk_renderer_free(ChunkRenderer* renderer);

#endif
