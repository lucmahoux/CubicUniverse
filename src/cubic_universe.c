#define CUB_DEV_STAGE
#define CUB_DEBUG

#include "block_list_loader.h"
#include "chunk.h"
#include "game.h"

void create_block_list() {
    GLfloat flower_VBO[] = {
        0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   // Diag 1
        1.0f,   0.0f,   1.0f,   1.0f,   0.0f,   0.0f,
        1.0f,   1.0f,   1.0f,   1.0f,   1.0f,   0.0f,
        0.0f,   1.0f,   0.0f,   0.0f,   1.0f,   0.0f,

        0.0f,   0.0f,   1.0f,   0.0f,   0.0f,   0.0f,   // Diag 2
        1.0f,   0.0f,   0.0f,   1.0f,   0.0f,   0.0f,
        1.0f,   1.0f,   0.0f,   1.0f,   1.0f,   0.0f,
        0.0f,   1.0f,   1.0f,   0.0f,   1.0f,   0.0f
    };
    GLuint flower_EBO[] = {
        0,  1,  2,  0,  2,  3,
        4,  5,  6,  4,  6,  7
    };
    BLloader_create_block_list_file();
    GLsizei VBO_flower_len = sizeof(flower_VBO) / sizeof(GLfloat);
    GLsizei EBO_flower_len = sizeof(flower_EBO) / sizeof(GLuint);
    BLloader_add_buffer(flower_VBO, VBO_flower_len,
                        flower_EBO, EBO_flower_len);
    BlockFlags flags = { .is_solid = false,
                         .has_gravity = false,
                         .is_opaque = false,
                         .is_cubic = true,
                         .has_bs_tex = false,
                         .is_bs_creator = false,
                         .has_states = false };
    uint8_t nb_states = 0;
    // Air
    BLloader_add_block("air", RT_NONE, 0, flags, nb_states, NULL, NULL);
    flags.is_solid = true;
    flags.is_opaque = true;
    // Diamond ore
    BLloader_add_block("diamond_ore", RT_DEFAULT, 0,
                        flags, nb_states, NULL, NULL);
    // Crafting table
    BLloader_add_block("crafting_table", RT_TOP | RT_SIDE | RT_FRONT, 0,
                        flags, nb_states, NULL, NULL);
    flags.has_bs_tex = true;
    flags.is_bs_creator = true;
    flags.has_states = true;
    nb_states = 1;
    bs_key bs_keys[] = { BS_WOOD_TYPE };
    bs_val bs_def_vals[] = { BSV_OAK };
    // Wood planks
    BLloader_add_block("planks", RT_DEFAULT, 0, flags, nb_states,
                        bs_keys, bs_def_vals);
    // Wood log
    // TODO: add BS_FACING
    BLloader_add_block("log", RT_TOP | RT_DEFAULT, 0, flags, nb_states,
                        bs_keys, bs_def_vals);
    flags.is_solid = false;
    flags.has_gravity = true;
    flags.is_opaque = true;
    flags.is_cubic = false;
    bs_keys[0] = BS_FLOWER_TYPE;
    bs_def_vals[0] = BSV_POPPY;
    // Flowers
    BLloader_add_block("flower", RT_DEFAULT, 0, flags, nb_states,
                        bs_keys, bs_def_vals);
    // Furnace (on / off)
    flags.is_solid = true;
    flags.has_gravity = false;
    flags.is_opaque = true;
    flags.is_bs_creator = false;
    bs_keys[0] = BS_LIT;
    bs_def_vals[0] = BSV_FALSE;
    BLloader_add_block("furnace", RT_TOP  | RT_SIDE | RT_FRONT, 0, flags,
                        nb_states, bs_keys, bs_def_vals);
}

int main(void) {
    Game game;
    create_block_list();
    game_init(&game, 1024, 768);
    game_start(&game);
    return EXIT_SUCCESS;
}
