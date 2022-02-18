#ifndef CUB_RENDER_BLOCKSTATES_H
#define CUB_RENDER_BLOCKSTATES_H

#include "render/block_utils.h"

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
cubBS_parser cub_BS_is_creator(cubBlockStateKey bsk);

/* Returns TRUE only if the blockstate 'bs' CREATES a totally different block
 * (e.g: BS_FLOWER_TYPE -> one value equals one flower, but the BS_COLOR
 *  only modifies the color of the block) */
bool cub_BS_is_total_creator(cubBlockStateKey bsk);

/* If the blockstate 'bs' MODIFIES the appearance of a block without changing
 * its nature (e.g: furnace VS furnace_lit), the function returns
 * a function pointer to a parser to get the suffix related to the state
 * modifier. Otherwise, it returns NULL. */
cubBS_parser cub_BS_is_modificator(cubBlockStateKey bsk);

/* Returns the number of possible values/modes for a TEXTURER blockstate
 * The default value, if the bs couldn't be identified, is 0 */
uint16_t cub_BS_TEX_get_nb_modes(cubBlockStateKey bsk);

#endif
