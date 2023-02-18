#ifndef CUB_BLOCK_BLOCKSTATES_H
#define CUB_BLOCK_BLOCKSTATES_H

#include "block_utils.h"

/* All the following functions simply fill the buffer with the string
 * name of the 'value' blockstate value.
 * It returns a pointer to the character following the last filled
 * character. If the value does not exist, returns NULL.
 * THERE MUST BE ENOUGH SPACE IN THE BUFFER :) */
char* BSCRE_wood_type(BlockStateValue value, char* buffer);
char* BSCRE_flower_type(BlockStateValue value, char* buffer);
char* BSCRE_color_type(BlockStateValue value, char* buffer);
char* BSCRE_double_plant_type(BlockStateValue value, char* buffer);

/* All the following functions simply fill the buffer with the string
 * name of the 'value' blockstate value.
 * It returns a pointer to 'buffer' or NULL if the value does not exist.
 * THERE MUST BE ENOUGH SPACE IN THE BUFFER :) */
char* BSMOD_LIT_suffix(BlockStateValue value, char* buffer);
char* BSMOD_AGE_suffix(BlockStateValue value, char* buffer);

/* If the blockstate 'bs' CREATES a block by modifying its NATURE
 * (= no textures can be found without it -> e.g: "wool.png" needs a
 * color prefix), the function returns a callback function to the
 * related value parser.
 * If the blockstate 'bs' is NOT a block creator, it returns NULL */
BS_parser BS_is_creator(BlockStateKey bsk);

/* Returns TRUE only if the blockstate 'bs' CREATES a totally different block
 * (e.g: BS_FLOWER_TYPE -> one value equals one flower, but the BS_COLOR
 *  only modifies the color of the block) */
bool BS_is_total_creator(BlockStateKey bsk);

/* If the blockstate 'bs' MODIFIES the appearance of a block without changing
 * its nature (e.g: furnace VS furnace_lit), the function returns
 * a function pointer to a parser to get the suffix related to the state
 * modifier. Otherwise, it returns NULL. */
BS_parser BS_is_modificator(BlockStateKey bsk);

/* Returns the number of possible values/modes for a TEXTURER blockstate
 * The default value, if the bs couldn't be identified, is 0 */
uint16_t BS_TEX_get_nb_modes(BlockStateKey bsk);

#endif
