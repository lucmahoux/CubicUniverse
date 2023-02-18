#ifndef CUB_TEXTURE_TYPES_H
#define CUB_TEXTURE_TYPES_H

#include "utils.h"

typedef union cubTextureAtlasQuad {
    uint32_t packed;
    struct cubUnpackedTextureAtlasQuad {
        uint32_t offsetX        : 5;
        uint32_t offsetY        : 16;
        uint32_t texture_type   : 3;
        uint32_t has_been_added : 1; // For texture adding
        uint32_t unused         : 7;
    } val;
} TexAtlasQuad;

typedef struct cubTextureAtlas {
    size_t fixed_width;
    size_t height;
    TexAtlasQuad last_16, last_32, last_64, last_128, last_256, last_512;
    GLuint atlas_texture_id;
    uint8_t* pixels; // Format is RGBA
} TextureAtlas;

#endif /* !CUB_TEXTURE_TYPES_H */
