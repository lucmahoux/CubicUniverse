#ifndef CUB_CHUNK_CHUNK_RENDERER_H
#define CUB_CHUNK_CHUNK_RENDERER_H

#include "chunk/chunk_loader.h"
#include "chunk/chunk_utils.h"
#include "chunk/chunk.h"
#include "utils/utils.h"

/* Must be called when the renderer has been setup.
 * It initializes the chunk_grid by loading the chunks to render*/
void chunk_renderer_init_grid(ChunkRenderer* renderer, BlockList* block_list);

/* Should be called when there is a need to load a brand new chunk grid.
 * It basically saves the current chunk grid on disk.
 * An example of a use case would be when a player teleports somewhere
 * or at the end of the game to free the chunks */
void chunk_renderer_unload_grid(ChunkRenderer* renderer);

/* Allocate all the memory required by the chunk renderer.
 * Should be called only once, at the start of the game.
 * It initiliazes the grid of chunk using chunk_renderer_init() function */
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

/* Calls 'subchunk_render_dfs' from the faces specified in
 * SC->visibility_graph[FID_ENTRANCES] to render the visible blocks */
void subchunk_render(ChunkRenderer* renderer, BlockRenderer* block_renderer,
                     Chunk* C, SubChunk* SC);

#endif
