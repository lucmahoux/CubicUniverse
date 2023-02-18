#include "blockstates.h"

char* BSCRE_wood_type(BlockStateValue value, char* buffer) {
    size_t offset = 0;
    switch (value) {
        case BSV_ACACIA:
            strcpy(buffer, "acacia"); offset = 6; break;
        case BSV_BIRCH:
            strcpy(buffer, "birch"); offset = 5; break;
        case BSV_DARK_OAK:
            strcpy(buffer, "dark_oak"); offset = 8; break;
        case BSV_JUNGLE:
            strcpy(buffer, "jungle"); offset = 6; break;
        case BSV_OAK:
            strcpy(buffer, "oak"); offset = 3; break;
        case BSV_SPRUCE:
            strcpy(buffer, "spruce"); offset = 6; break;
        default:
            return NULL;
    }
    return buffer + offset;
}

char* BSCRE_flower_type(BlockStateValue value, char* buffer) {
    size_t offset = 0;
    switch (value) {
        case BSV_POPPY:
            strcpy(buffer, "poppy"); offset = 5; break;
        case BSV_ORCHID:
            strcpy(buffer, "blue_orchid"); offset = 11; break;
        case BSV_ALLIUM:
            strcpy(buffer, "allium"); offset = 6; break;
        case BSV_AZURE_BLUET:
            strcpy(buffer, "azure_bluet"); offset = 11; break;
        case BSV_TULIP_RED:
            strcpy(buffer, "red_tulip"); offset = 9; break;
        case BSV_TULIP_ORANGE:
            strcpy(buffer, "orange_tulip"); offset = 12; break;
        case BSV_TULIP_WHITE:
            strcpy(buffer, "white_tulip"); offset = 11; break;
        case BSV_TULIP_PINK:
            strcpy(buffer, "pink_tulip"); offset = 10; break;
        case BSV_OXEYE:
            strcpy(buffer, "oxeye_daisy"); offset = 11; break;
        case BSV_CORNFLOWER:
            strcpy(buffer, "cornflower"); offset = 10; break;
        case BSV_LILY_OF_THE_VALLEY:
            strcpy(buffer, "lily_of_the_valley"); offset = 18; break;
        default:
            return NULL;
    }
    return buffer + offset;
}

char* BSCRE_color_type(BlockStateValue value, char* buffer) {
    size_t offset = 0;
    switch (value) {
        case BSV_WHITE:
            strcpy(buffer, "white"); offset = 5; break;
        case BSV_ORANGE:
            strcpy(buffer, "orange"); offset = 6; break;
        case BSV_MAGENTA:
            strcpy(buffer, "magenta"); offset = 7; break;
        case BSV_LIGHT_BLUE:
            strcpy(buffer, "light_blue"); offset = 10; break;
        case BSV_YELLOW:
            strcpy(buffer, "yellow"); offset = 6; break;
        case BSV_LIME:
            strcpy(buffer, "lime"); offset = 4; break;
        case BSV_PINK:
            strcpy(buffer, "pink"); offset = 4; break;
        case BSV_GRAY:
            strcpy(buffer, "gray"); offset = 4; break;
        case BSV_LIGHT_GRAY:
            strcpy(buffer, "light_gray"); offset = 10; break;
        case BSV_CYAN:
            strcpy(buffer, "cyan"); offset = 4; break;
        case BSV_PURPLE:
            strcpy(buffer, "purple"); offset = 6; break;
        case BSV_BLUE:
            strcpy(buffer, "blue"); offset = 4; break;
        case BSV_BROWN:
            strcpy(buffer, "brown"); offset = 5; break;
        case BSV_GREEN:
            strcpy(buffer, "green"); offset = 5; break;
        case BSV_RED:
            strcpy(buffer, "red"); offset = 3; break;
        case BSV_BLACK:
            strcpy(buffer, "black"); offset = 5; break;
        default:
            return NULL;
    }
    return buffer + offset;
}

char* BSCRE_double_plant_type(BlockStateValue value, char* buffer) {
    size_t offset = 0;
    switch (value) {
        case BSV_SUNFLOWER:
            strcpy(buffer, "sunflower"); offset = 9; break;
        case BSV_LILAC:
            strcpy(buffer, "lilac"); offset = 5; break;
        case BSV_GRASS:
            strcpy(buffer, "double_tallgrass"); offset = 16; break;
        case BSV_FERN:
            strcpy(buffer, "large_fern"); offset = 10; break;
        case BSV_ROSE:
            strcpy(buffer, "rose_bush"); offset = 9; break;
        case BSV_PEONY:
            strcpy(buffer, "peony"); offset = 5; break;
        default:
            return NULL;
    }
    return buffer + offset;
}

BS_parser BS_is_creator(BlockStateKey bsk) {
    switch (bsk) {
        case BS_DOUBLE_PLANT_TYPE:
            return BSCRE_double_plant_type;
        case BS_FLOWER_TYPE:
            return BSCRE_flower_type;
        case BS_COLOR:
            return BSCRE_color_type;
        case BS_WOOD_TYPE:
            return BSCRE_wood_type;
        default:
            return NULL;
    }
    return NULL;
}

bool BS_is_total_creator(BlockStateKey bsk) {
    switch (bsk) {
        case BS_DOUBLE_PLANT_TYPE:
        case BS_FLOWER_TYPE:
            return true;
        default:
            return false;
    }
    return false;
}

char* BSMOD_LIT_suffix(BlockStateValue value, char* buffer) {
    switch (value) {
        case BSV_FALSE:
            buffer[0] = '\0'; break;
        case BSV_TRUE:
            strcpy(buffer, "_on"); break;
        default:
            return NULL;
    }
    return buffer;
}

char* BSMOD_AGE_suffix(BlockStateValue value, char* buffer) {
    switch (value) {
        case BSV_STAGE0:
            strcpy(buffer, "_stage0"); break;
        case BSV_STAGE1:
            strcpy(buffer, "_stage1"); break;
        case BSV_STAGE2:
            strcpy(buffer, "_stage2"); break;
        case BSV_STAGE3:
            strcpy(buffer, "_stage3"); break;
        default:
            return NULL;
    }
    return buffer;
}

BS_parser BS_is_modificator(BlockStateKey bsk) {
    switch (bsk) {
        case BS_LIT:
            return BSMOD_LIT_suffix;
        case BS_AGE:
            return BSMOD_AGE_suffix;
        default:
            return NULL;
    }
    return NULL;
}

uint16_t BS_TEX_get_nb_modes(BlockStateKey bsk) {
    switch (bsk) {
        case BS_LIT:
            return 2;
        case BS_AGE:
            return 4;
        case BS_DOUBLE_PLANT_TYPE:
        case BS_WOOD_TYPE:
            return 6;
        case BS_FLOWER_TYPE:
            return 11;
        case BS_COLOR:
            return 16;
        default:
            return 0;
    }
    return 0;
}
