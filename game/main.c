#define CUB_DEV_STAGE
#include "game/game.h"
#include "render/chunk.h"

void create_block_list() {
    GLfloat cube_VBO[] = {
        0.0f,   1.0f,   1.0f,   0.0f,   0.0f,   1.0f,   // Face 1 - Top
        1.0f,   1.0f,   1.0f,   1.0f,   0.0f,   1.0f,
        1.0f,   1.0f,   0.0f,   1.0f,   1.0f,   1.0f,
        0.0f,   1.0f,   0.0f,   0.0f,   1.0f,   1.0f,

        0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   // Face 2 - Bottom
        1.0f,   0.0f,   0.0f,   1.0f,   0.0f,   2.0f,
        1.0f,   0.0f,   1.0f,   1.0f,   1.0f,   2.0f,
        0.0f,   0.0f,   1.0f,   0.0f,   1.0f,   2.0f,

        0.0f,   0.0f,   1.0f,   0.0f,   0.0f,   3.0f,   // Face 3 - Front
        1.0f,   0.0f,   1.0f,   1.0f,   0.0f,   3.0f,
        1.0f,   1.0f,   1.0f,   1.0f,   1.0f,   3.0f,
        0.0f,   1.0f,   1.0f,   0.0f,   1.0f,   3.0f,

        1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   // Face 4 - Back
        0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   4.0f,
        0.0f,   1.0f,   0.0f,   1.0f,   1.0f,   4.0f,
        1.0f,   1.0f,   0.0f,   0.0f,   1.0f,   4.0f,

        0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   5.0f,   // Face 5 - Left
        0.0f,   0.0f,   1.0f,   1.0f,   0.0f,   5.0f,
        0.0f,   1.0f,   1.0f,   1.0f,   1.0f,   5.0f,
        0.0f,   1.0f,   0.0f,   0.0f,   1.0f,   5.0f,

        1.0f,   0.0f,   1.0f,   0.0f,   0.0f,   6.0f,   // Face 6 - Right
        1.0f,   0.0f,   0.0f,   1.0f,   0.0f,   6.0f,
        1.0f,   1.0f,   0.0f,   1.0f,   1.0f,   6.0f,
        1.0f,   1.0f,   1.0f,   0.0f,   1.0f,   6.0f
    };
    GLuint cube_EBO[] = {
        0,  1,  2,  0,  2,  3,  // Face 1
        4,  5,  6,  4,  6,  7,  // Face 2
        8,  9,  10, 8,  10, 11, // Face 3
        12, 13, 14, 12, 14, 15, // Face 4
        16, 17, 18, 16, 18, 19, // Face 5
        20, 21, 22, 20, 22, 23  // Face 6
    };
    GLfloat flower_VBO[] = {
        0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   // Diag 1
        1.0f,   0.0f,   1.0f,   1.0f,   0.0f,   1.0f,
        1.0f,   1.0f,   1.0f,   1.0f,   1.0f,   1.0f,
        0.0f,   1.0f,   0.0f,   0.0f,   1.0f,   1.0f,

        0.0f,   0.0f,   1.0f,   0.0f,   0.0f,   1.0f,   // Diag 2
        1.0f,   0.0f,   0.0f,   1.0f,   0.0f,   1.0f,
        1.0f,   1.0f,   0.0f,   1.0f,   1.0f,   1.0f,
        0.0f,   1.0f,   1.0f,   0.0f,   1.0f,   1.0f
    };
    GLuint flower_EBO[] = {
        0,  1,  2,  0,  2,  3,
        4,  5,  6,  4,  6,  7
    };
    BLloader_create_block_list_file();
    GLsizei VBO_len = sizeof(cube_VBO) / sizeof(GLfloat);
    GLsizei EBO_len = sizeof(cube_EBO) / sizeof(GLuint);
    GLsizei VBO_flower_len = sizeof(flower_VBO) / sizeof(GLfloat);
    GLsizei EBO_flower_len = sizeof(flower_EBO) / sizeof(GLuint);
    BLloader_add_buffer(cube_VBO, VBO_len,
                            cube_EBO, EBO_len);
    BLloader_add_buffer(flower_VBO, VBO_flower_len,
                            flower_EBO, EBO_flower_len);
    blockInfo block_info = { .is_solid = false,
                                .has_gravity = false,
                                .has_bs_tex = false,
                                .is_bs_creator = false,
                                .has_states = false,
                                .nb_states = 0 };
    // Air
    BLloader_add_block("air", RT_NONE, 0, block_info, NULL, NULL);
    block_info.is_solid = true;
    // Diamond ore
    BLloader_add_block("diamond_ore", RT_DEFAULT, 0,
                            block_info, NULL, NULL);
    // Crafting table
    BLloader_add_block("crafting_table", RT_TOP | RT_SIDE | RT_FRONT, 0,
                            block_info, NULL, NULL);
    block_info.has_bs_tex = true;
    block_info.is_bs_creator = true;
    block_info.has_states = true;
    block_info.nb_states = 1;
    bs_key bs_keys[] = { BS_WOOD_TYPE };
    bs_val bs_def_vals[] = { BSV_OAK };
    // Wood planks
    BLloader_add_block("planks", RT_DEFAULT, 0,
                            block_info, bs_keys, bs_def_vals);
    // Wood logs
    // TODO: add BS_FACING
    BLloader_add_block("log", RT_TOP | RT_DEFAULT, 0,
                            block_info, bs_keys, bs_def_vals);
    block_info.is_solid = false;
    block_info.has_gravity = true;
    bs_keys[0] = BS_FLOWER_TYPE;
    bs_def_vals[0] = BSV_POPPY;
    // Flowers
    BLloader_add_block("flower", RT_DEFAULT, 1,
                            block_info, bs_keys, bs_def_vals);
    // Furnace (on / off)
    block_info.is_solid = true;
    block_info.has_gravity = false;
    block_info.is_bs_creator = false;
    bs_keys[0] = BS_LIT;
    bs_def_vals[0] = BSV_FALSE;
    BLloader_add_block("furnace", RT_TOP  | RT_SIDE | RT_FRONT, 0,
                            block_info, bs_keys, bs_def_vals);
}

int main(void) {
    cubGame game;
    create_block_list();
    cub_game_init(&game, 1024, 768);
    cub_game_start(&game);
    return EXIT_SUCCESS;
}
