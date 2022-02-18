#include "render/block_utils.h"

const uint8_t CUB_BSV_LEN[] = {
    4,      // BS_FACING
    4,      // BS_AGE
    2,      // BS_LIT
    16,     // BS_COLOR
    2,      // BS_HALF
    11,     // BS_FLOWER_TYPE
    6,      // BS_DOUBLE_FLOWER_TYPE
    6       // BS_WOOD_TYPE
};

cubBP_elt* cub_block_BP_elt(cubBlockData* data, cub_block_t id,
                            cub_bs_val* states) {
    cubBP_elt* elt = malloc(sizeof(cubBP_elt));
    if (!elt) errx(1, "cub_block_BP_elt: Malloc failed!");
    elt->block.states = malloc(sizeof(cub_bs_val) * data->block_info.nb_states);
    for (uint8_t i = 0; i < data->block_info.nb_states; ++i)
        elt->block.states[i] = states[i];
    if (!elt->block.states) errx(1, "cub_block_BP_elt: Malloc failed!");
    elt->block.id = id;
    elt->nb_blocks = 1;
    return elt;
}
