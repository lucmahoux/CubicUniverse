#ifndef CUB_BLOCK_BLOCK_RENDERER_H
#define CUB_BLOCK_BLOCK_RENDERER_H

#include "block_utils.h"
#include "block_list_loader.h"
#include "blockstates.h"
#include "camera.h"
#include "data_struct/matrix.h"
#include "shader.h"
#include "texture_atlas.h"
#include "types/chunk.h"

// -------- Rendering functions --------
/* Should be called only once to set up the block renderer and load the
 * block list */
void block_setup_renderer_and_block_list(BlockRenderer* block_renderer,
                                         BlockList* block_list);

/* Add a CUBIC block face to the rendering cache.
 * If the cache has been rendered as a result of the call, the function returns
 * TRUE, FALSE otherwise.
 * The output should be used to reapply the default shader program as
 * the cubic shader program would have been applied if the cache was rendered
 * (output to TRUE) */
bool block_cubic_add_to_cache(CubicBlockRenderer* cubic, BlockState* BS,
                              FaceID face, SubChunkCoords position);

/* Render & Flush the rendering cache of CUBIC block faces */
void block_cubic_flush_cache(CubicBlockRenderer* cubic);

/* Should be called when the block to render is a CUSTOM block.
 * [!] No shader program is used during the procedure, therefore it is
 * assumed that the CUSTOM shader is used before calling the function [!]*/
void block_custom_render(CustomBlockRenderer* custom, BlockState* BS,
                         SubChunkCoords position);

/* Should be called only at the end of the game */
void block_free_renderer(BlockRenderer* renderer);

/* Loads a texture pack - the texture atlas must be freed upon calling */
void block_load_texture_pack(BlockRenderer* block_renderer,
                             BlockList* block_list);

#endif
