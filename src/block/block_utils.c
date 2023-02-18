#include "block_utils.h"

const uint8_t CUB_BSV_LEN[] = {
    BSV_FACING_LEN,
    BSV_AGE_LEN,
    BSV_LIT_LEN,
    BSV_COLOR_LEN,
    BSV_HALF_LEN,
    BSV_FLOWER_TYPE_LEN,
    BSV_DOUBLE_PLANT_TYPE_LEN,
    BSV_WOOD_TYPE_LEN
};

void block_set_BS_uid(BlockState* BS) {
    BSuid bs_uid = BS->block->origin_BS_uid;
    BSuid bsv_combinations = 1;
    for (uint8_t bs_id = BS->block->nb_states; bs_id > 0; --bs_id) {
        bs_uid += BS->states[bs_id - 1] * bsv_combinations;
        bsv_combinations *= CUB_BSV_LEN[BS->block->bs_keys[bs_id - 1]];
    }
    BS->BS_uid = bs_uid;
}

BP_elt* block_BP_elt_new(BlockState* BS) {
    const char fname[] = "block_BP_elt_new";
    BP_elt* elt = utils_malloc(sizeof(BP_elt), fname);
    elt->BS.states = utils_malloc(sizeof(bs_val) * BS->block->nb_states, fname);
    for (uint8_t i = 0; i < BS->block->nb_states; ++i)
        elt->BS.states[i] = BS->states[i];
    elt->BS.block = BS->block;
    elt->nb_blocks = 1;
    block_set_BS_uid(BS);
    return elt;
}

void block_BP_elt_free(BP_elt* bp_elt) {
    free(bp_elt->BS.states);
    free(bp_elt);
}
