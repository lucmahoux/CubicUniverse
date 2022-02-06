#define CUB_DEV_STAGE
#include "struct/block.h"

extern cubBlockList CUB_ALL_BLOCKS;

int main() {
    //cub_block_new_block("diamond_ore", 1, CUBE_SAME_FACES);
    cub_block_load_block_list();
    for (size_t i = 0; i < CUB_ALL_BLOCKS.nb_blocks; ++i) {
        cubBlockData* block = &CUB_ALL_BLOCKS.blocks[i];
        printf("Name: %s - ID: %i - Type: %i\n",
                block->name, block->id, block->type);
    }
    cub_block_load_texture_pack();
    //return cub_graphics_open_window();
    return EXIT_SUCCESS;
}
