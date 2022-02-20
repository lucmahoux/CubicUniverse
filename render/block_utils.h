#ifndef CUB_RENDER_BLOCK_UTILS_H
#define CUB_RENDER_BLOCK_UTILS_H

#include "utils/open_gl.h"
#include "utils/utils.h"

#define bs_key uint8_t
#define bs_val uint8_t
#define MAX_BLOCK_STRLEN 32
#define AIR_ID 0
#define DEFAULT_VAO_ID 0
#define BLOCK_LIST_FILE "block-list.cubic"
/* If the blockstate texturer exists, then its ID in the 'bs_keys' of a
 * cubBlockData must be 0! */
#define BS_TEX_ID 0

typedef enum blockStateKey {
    BS_FACING,  // Orientation of the block
    BS_AGE,     // Age longevity of a crop
    BS_LIT,     // When a block can be "turned on/off" (e.g. furnace)
    BS_COLOR,   // color suffixes for all textures
    BS_HALF,    // For objects composed of 2 blocks (e.g. a door, double plant)
    BS_FLOWER_TYPE,
    BS_DOUBLE_PLANT_TYPE,
    BS_WOOD_TYPE
} blockStateKey;

typedef enum blockStateValue {
    // BS_FACING
    BSV_NORTH = 0, BSV_SOUTH, BSV_EAST, BSV_WEST,
    // BS_AGE
    BSV_STAGE0 = 0, BSV_STAGE1, BSV_STAGE2, BSV_STAGE3,
    // BS_LIT
    BSV_FALSE = 0, BSV_TRUE,
    // BS_COLOR
    BSV_WHITE = 0, BSV_ORANGE, BSV_MAGENTA, BSV_LIGHT_BLUE, BSV_YELLOW,
    BSV_LIME, BSV_PINK, BSV_GRAY, BSV_LIGHT_GRAY, BSV_CYAN, BSV_PURPLE,
    BSV_BLUE, BSV_BROWN, BSV_GREEN, BSV_RED, BSV_BLACK,
    // BS_HALF
    BSV_LOWER = 0, BSV_UPPER,
    // BS_FLOWER_TYPE
    BSV_POPPY = 0, BSV_ORCHID, BSV_ALLIUM, BSV_AZURE_BLUET, BSV_TULIP_RED,
    BSV_TULIP_ORANGE, BSV_TULIP_WHITE, BSV_TULIP_PINK, BSV_OXEYE,
    BSV_CORNFLOWER, BSV_LILY_OF_THE_VALLEY,
    // BS_DOUBLE_PLANT_TYPE
    BSV_SUNFLOWER = 0, BSV_LILAC, BSV_GRASS, BSV_FERN, BSV_ROSE, BSV_PEONY,
    // BS_WOOD_TYPE
    BSV_ACACIA = 0, BSV_BIRCH, BSV_DARK_OAK, BSV_JUNGLE, BSV_OAK, BSV_SPRUCE
} blockStateValue;

extern const uint8_t CUB_BSV_LEN[];

typedef char* (*BS_parser) (blockStateValue, char*);

// Define how the textures file names must be formed.
// It is also used to bind the correct textures when using the default VAO
typedef enum renderType {
    RT_NONE             =   0x00,   // no texture (air)
    RT_TOP              =   0x01,   // _top suffix
    RT_BOTTOM           =   0x02,   // _bottom suffix
    RT_FRONT            =   0x04,   // _front suffix
    RT_BACK             =   0x08,   // _back suffix
    RT_LEFT             =   0x10,   // _left suffix
    RT_RIGHT            =   0x20,   // _right suffix
    RT_SIDE             =   0x40,   // _side suffix
    RT_DEFAULT          =   0x80    // no suffix
} renderType;

typedef struct blockInfo {
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
} blockInfo;

typedef struct blockData {
    renderType render_type;
    char name[MAX_BLOCK_STRLEN];
    block_t id;
    blockInfo block_info;
    uint8_t nb_tex_draw; // Nbr of different textures to load on draw call
    uint8_t VAO_id; // ID of the VAO used to render the block
    // Set to NULL if the block does not have a BS name modifier.
    // Otherwise, point to the function used to parse its values.
    BS_parser bs_name_parser;
    GLuint* textures;
    bs_key* bs_keys;
    bs_val* bs_default_values;
} blockData;

typedef struct blockList {
    // Array of all blocks
    blockData* blocks;
    // Total number of blocks
    size_t nb_blocks;
} blockList;

typedef enum blockFaceIndex {
    CUB_DEFAULT_START_ID,
    CUB_TOP_ID              =       CUB_DEFAULT_START_ID,
    CUB_BOTTOM_ID,
    CUB_SIDE_START_ID,
    CUB_FRONT_ID            =       CUB_SIDE_START_ID,
    CUB_BACK_ID,
    CUB_LEFT_ID,
    CUB_RIGHT_ID,
    CUB_SIDE_END_ID         =       CUB_RIGHT_ID,
    CUB_DEFAULT_END_ID      =       CUB_SIDE_END_ID
} blockFaceIndex;

typedef struct blockState {
    block_t id;
    bs_val* states;
} blockState;

typedef struct blockPaletteElement {
    blockState block;
    // Nbr of blocks referencing this palette element in the subchunk
    uint16_t nb_blocks;
} BP_elt;

typedef struct blockRenderer {
    // All the buffers used to render the blocks
    renderBufferObject* buffer_objs;
    uint8_t nb_buffers;
    GLuint shader_program;
    // Use the cubBlockFaceIndex enum to access the texture associated to
    // a certain face of the cube - works only for the default VAO.
    GLint tex_locations[6];
    GLint model_uni_loc; // Uniform location of model_matrix
    mat4 model_matrix;
    blockList block_list;
} blockRenderer;

// ---------- Useful Macros ----------- //
#define CUB_BLOCK_DATA(renderer, id) (renderer->block_list.blocks + id)
#define CUB_NB_STATES(renderer, id) CUB_BLOCK_DATA(renderer, id)->block_info.nb_states
#define CUB_BSK_TEX(block_data) block_data->bs_keys[BS_TEX_ID]

/* Creates a new BP_elt by allocating its memory.
 * The default number of blocks is set to 1! */
BP_elt* block_BP_elt(blockData* data, block_t id,
                            bs_val* states);


#endif
