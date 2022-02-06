#define CUB_DEV_STAGE
#include "game/game.h"

int main(void) {
    //cub_block_new_block("crafting_table", 1, CUBE_UNIQUE_FACES);
    cubGame game;
    cub_game_init(&game, 1024, 768);
    cub_game_start(&game);
    return EXIT_SUCCESS;
}
