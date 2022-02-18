#ifndef CUB_RENDER_BLOCK_LIST_LOADER
#define CUB_RENDER_BLOCK_LIST_LOADER

#include "render/block_utils.h"
#include "render/blockstates.h"

/* Determine the number of .png textures to load for one draw call
 * This number is equivalent to the field cubBlockData.nb_tex_draw */
uint8_t cub_BLloader_count_draw_tex(cubRenderType type);

/* Allocates the correct amount of memory to block->textures based
 * on the number of textures to load per draw call and the number
 * of "modes" its blockstates(BS) implies.
 * e.g: for a flower, nb_tex_draw = 1 (render_type == RT_DEFAULT)
 * but the BS BS_FLOWER_TYPE has 11 modes.
 * Therefore, the memory allocated must be nb_tex_draw * 11 * sizeof(GLuint) */
void cub_BLloader_allocate_tex_mem(cubBlockData* block);

/* Initialize the cubRenderBufferObject render_obj with the VBO & EBO buffers */
void cub_BLloader_create_VAO(GLfloat* VBO, GLuint* EBO,
                             GLsizei VBO_len, GLsizei EBO_len,
                             cubRenderBufferObject* render_obj);

/* Load the header of the block list file & set the 'nb_buffers' & 'render_objs'
 * values accordingly.
 * The memory for the array block_list->blocks is also allocated */
void cub_BLloader_load_header(cubBlockList* block_list, uint8_t* nb_buffers,
                              cubRenderBufferObject** render_objs, FILE* fp);

/* Load the body (= the list of blocks) of the block list file */
void cub_BLloader_load_blocks(cubBlockList* block_list, FILE* fp);

/* Load the block_list & initialize it. It sets the pointer 'nb_buffers' to
 * the number of generated render_objs. */
void cub_BLloader_load(cubBlockList* block_list, uint8_t* nb_buffers,
                        cubRenderBufferObject** render_objs);

#ifdef CUB_DEV_STAGE
// [!] ALL the buffers MUST be added FIRST -
//     only then it is possible to add blocks [!]

/* Create or reset the block_list file if it exists */
void cub_BLloader_create_block_list_file();

/* Adds a new rendering buffer in the block_list file */
void cub_BLloader_add_buffer(GLfloat* VBO, uint16_t VBO_len,
                             GLuint* EBO, uint8_t EBO_len);

/* Adds a new block to the block_list file */
void cub_BLloader_add_block(const char* name, cubRenderType render_type,
                            uint8_t VAO_id, cubBlockInfo block_info,
                            cub_bs_key* bs_keys,
                            cub_bs_val* bs_default_values);
#endif

#endif
