#ifndef CUB_RENDER_BLOCKSTATES_H
#define CUB_RENDER_BLOCKSTATES_H

#include "utils/utils.h"

typedef enum cubBlockState {
    BS_FACING,  // Orientation of the block
    BS_AGE,     // Age longevity of a crop
    BS_LIT,     // When a block can be "turned on/off" (e.g. furnace)
    BS_COLOR,   // color suffixes for all textures
    BS_HALF,    // For objects composed of 2 blocks (e.g. a door, double plant)
    BS_FLOWER_TYPE,
    BS_DOUBLE_PLANT_TYPE,
    BS_WOOD_TYPE
} cubBlockState;

typedef enum cubBlockStateValue {
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
} cubBlockStateValue;

typedef char* (*cubBS_parser) (cubBlockStateValue, char*);

/* All the following functions simply fill the buffer with the string
 * name of the 'value' blockstate value.
 * It returns a pointer to the character following the last filled
 * character. If the value does not exist, returns NULL.
 * THERE MUST BE ENOUGH SPACE IN THE BUFFER :) */
char* cub_BSCRE_wood_type(cubBlockStateValue value, char* buffer);
char* cub_BSCRE_flower_type(cubBlockStateValue value, char* buffer);
char* cub_BSCRE_color_type(cubBlockStateValue value, char* buffer);
char* cub_BSCRE_double_plant_type(cubBlockStateValue value, char* buffer);

/* All the following functions simply fill the buffer with the string
 * name of the 'value' blockstate value.
 * It returns a pointer to 'buffer' or NULL if the value does not exist.
 * THERE MUST BE ENOUGH SPACE IN THE BUFFER :) */
char* cub_BSMOD_LIT_suffix(cubBlockStateValue value, char* buffer);
char* cub_BSMOD_AGE_suffix(cubBlockStateValue value, char* buffer);

/* If the blockstate 'bs' CREATES a block by modifying its NATURE
 * (= no textures can be found without it -> e.g: "wool.png" needs a
 * color prefix), the function returns a callback function to the
 * related value parser.
 * If the blockstate 'bs' is NOT a block creator, it returns NULL */
cubBS_parser cub_BS_is_creator(cubBlockState bs);

/* Returns TRUE only if the blockstate 'bs' CREATES a totally different block
 * (e.g: BS_FLOWER_TYPE -> one value equals one flower, but the BS_COLOR
 *  only modifies the color of the block) */
bool cub_BS_is_total_creator(cubBlockState bs);

/* If the blockstate 'bs' MODIFIES the appearance of a block without changing
 * its nature (e.g: furnace VS furnace_lit), the function returns
 * a function pointer to a parser to get the suffix related to the state
 * modifier. Otherwise, it returns NULL. */
cubBS_parser cub_BS_is_modificator(cubBlockState bs);

/* Returns the number of possible values/modes for a TEXTURER blockstate
 * The default value, if the bs couldn't be identified, is 0 */
uint16_t cub_BS_TEX_get_nb_modes(cubBlockState bs);

#endif
