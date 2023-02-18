#ifndef CUB_BLOCK_BLOCK_UTILS_H
#define CUB_BLOCK_BLOCK_UTILS_H

#include "types/block.h"

/* Set the BlockState UID (BS->BS_uid field) of the given blockstate */
void block_set_BS_uid(BlockState* BS);

/* Creates a new BP_elt by allocating its memory.
 * The default number of blocks is set to 1! */
BP_elt* block_BP_elt_new(BlockState* BS);

/* Frees a block palette element */
void block_BP_elt_free(BP_elt* bp_elt);

#endif
