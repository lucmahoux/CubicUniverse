#ifndef CUB_RENDER_BLOCK_H
#define CUB_RENDER_BLOCK_H

#include "render/block_utils.h"
#include "render/block_list_loader.h"
#include "render/blockstates.h"
#include "utils/shader.h"

// -------- Rendering functions --------

/* Returns the cubBlockData related to the given block 'id' */
blockData* block_get_data(blockRenderer* renderer, block_t id);

/* Should be called only once to set up the block renderer */
void block_setup_renderer(blockRenderer* renderer);

/* Update the texture uniform identified by 'tex_loc'.
 * Binds the texture unit and set this uniform to 'gl_tex_nbr' */
void block_update_tex_uniform(GLuint tex_loc, uint8_t tex_id,
                                    uint8_t gl_tex_nbr);

/* Setup the textures uniforms of the shader if using the default VAO.
 * That means that tex_[top|bottom|front|back|left|right] uniforms are set
 * based on the cubRenderType of the given block */
void block_set_tex_default(blockRenderer* renderer, blockData* block,
                                blockState* bs);

/* Called when the block to render does NOT use the default VAO.
 * Textures uniforms are simple assigned in a linear manner to the
 * textures list of 'block' with respect to the order:
 * TOP -> BOTTOM -> FRONT -> BACK -> LEFT -> RIGHT */
void block_set_tex_custom(blockRenderer* renderer, blockData* block,
                                blockState* bs);

/* When calling this, it is assumed that the correct shader program
 * has been bound before the call 
 * Calls either cub_block_set_tex_default() if the bp_elt uses the default VAO,
 * or cub_block_set_custom() */
void block_render(blockRenderer* renderer, blockState* bs,
                        vec3 position);

/* Should be called only at the end of the game */
void block_free_renderer(blockRenderer* renderer);

// -------- Loading functions --------

/* Loads the texture formed by the concatenation of (prefix, ext, suffix)
 * and assigns the OpenGL texture buffer ID to *texture_id. */
void block_load_texture(char* buffer, const char* prefix, const char* ext,
                            const char* suffix, GLuint* texture_id);

/* Does exactly the same thing as cub_block_load_texture() but if the
 * texture file does NOT EXIST, then *tex_id = *def_tex_id. */
void block_try_load_texture(char* buffer, const char* prefix,
                                const char* ext, const char* suffix,
                                GLuint* tex_id, GLuint* def_tex_id);

/* Goes through all the possible render_type and load the textures
 * accordingly via calls to cub_block_load_texture() */
void block_tex_load_from_render_type(char* buffer, const char* prefix,
                                         const char* suffix,
                                         renderType render_type,
                                         GLuint* tex_id);

/* Loads the textures of a block which has a blockstate texturer "Modificator".
 * It means that the BS modifies the appearance of the block without changing
 * its nature (e.g. furnace off/on) */
void block_load_texture_BS_modificator(blockData* block);

/* Loads the textures of a block which has a blockstate texturer "Creator".
 * It means that the BS modifies the NATURE of a block
 * (e.g. acacia_planks VS dark_oak_planks) */
void block_load_texture_BS_creator(blockData* block);

/* Loads a texture pack */
void block_load_texture_pack(blockList* block_list);

/* Sould be called everytime the texture pack is changed */
void block_free_all_textures(blockList* block_list);

#endif
