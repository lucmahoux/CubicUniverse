#ifndef CUB_RENDER_BLOCK_LIST_LOADER
#define CUB_RENDER_BLOCK_LIST_LOADER

#include "render/blockstates.h"
#include "utils/utils.h"
#include "utils/open_gl.h"

#define BLOCK_LIST_FILE "block-list.cubic"
#define CUB_MAX_BLOCK_STRLEN 32
/* If the blockstate texturer exists, then its ID in the 'bs_keys' of a
 * cubBlockData must be 0! */
#define CUB_BS_TEX_ID 0
#define BS_TEX(block_ptr) block_ptr->bs_keys[CUB_BS_TEX_ID]
#define CUB_DEFAULT_VAO_ID 0
#define cub_bs_key uint8_t
#define cub_bs_val uint8_t

// Define how the textures file names must be formed.
// It is also used to bind the correct textures when using the default VAO
typedef enum cubRenderType {
    RT_NONE             =   0x00,   // no texture (air)
    RT_TOP              =   0x01,   // _top suffix
    RT_BOTTOM           =   0x02,   // _bottom suffix
    RT_FRONT            =   0x04,   // _front suffix
    RT_BACK             =   0x08,   // _back suffix
    RT_LEFT             =   0x10,   // _left suffix
    RT_RIGHT            =   0x20,   // _right suffix
    RT_SIDE             =   0x40,   // _side suffix
    RT_DEFAULT          =   0x80    // no suffix
} cubRenderType;

typedef struct cubBlockInfo {
    // Does the player collide with the block?
    uint8_t is_solid : 1;
    // Can it stand in the air without support?
    uint8_t has_gravity : 1;
    // Does it have a blockstate modifying its textures ("bs texturer")?
    uint8_t has_bs_tex : 1;
    // Only if has_bs_tex is TRUE, Is this BS a block CREATOR (!= MODIFICATOR)?
    uint8_t is_bs_creator : 1;
    // Does it have BSs? (including the bs_name_mod which must be the first)
    uint8_t has_states : 1;
    // If has_states is TRUE, how many are there?
    uint8_t nb_states : 3;
} cubBlockInfo;

typedef struct cubBlockData {
    cubRenderType render_type;
    char name[CUB_MAX_BLOCK_STRLEN];
    cub_block_t id;
    cubBlockInfo block_info;
    uint8_t nb_tex_draw; // Nbr of different textures to load on draw call
    uint8_t VAO_id; // ID of the VAO used to render the block
    // Set to NULL if the block does not have a BS name modifier.
    // Otherwise, point to the function used to parse its values.
    cubBS_parser bs_name_parser;
    GLuint* textures;
    cub_bs_key* bs_keys;
    cub_bs_val* bs_default_values;
} cubBlockData;

typedef struct cubBlockList {
    // Array of all blocks
    cubBlockData* blocks;
    // Total number of blocks
    size_t nb_blocks;
} cubBlockList;

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
