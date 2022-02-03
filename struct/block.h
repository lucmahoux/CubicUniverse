#ifndef CUB_STRUCT_BLOCK_H
#define CUB_STRUCT_BLOCK_H

#include "utils/utils.h"
#include "graphics/open_gl.h"

#define cub_block_t uint32_t
#define CUB_BLOCK_FILE_BUFFER 16
#define CUB_BLOCK_LIST_PATH "osef pour l'instant"

// Define how the block must be textured
typedef enum cubBlockTextureType {
    CUBE_UNIQUE_FACES,
    CUBE_TOP_BOTTOM,
    CUBE_SAME_FACES
} cubBlockTextureType;

typedef struct cubBlockData {
    cubBlockTextureType type;
    // bool is_solid; -> later
    char name[CUB_BLOCK_FILE_BUFFER];
    cub_block_t id;
    GLuint* textures_id;
} cubBlockData;

typedef struct cubBlockList {
    // Array of alls blocks
    cubBlockData* blocks;
    // Total number of blocks
    size_t nb_blocks;
} cubBlockList;

cubBlockList CUB_ALL_BLOCKS;

/* Sould be called only once, at the start of the game */
void cub_block_load_block_list();

/* Loads a texture pack */
void cub_block_load_texture_pack();

void cub_block_load_texture_same_faces(char* buffer, size_t name_len,
                                        GLuint* tex_id_block);
void cub_block_load_texture_top_bottom_faces(char* buffer, size_t name_len,
                                             GLuint* tex_id_block);
void cub_block_load_texture_unique_faces(char* buffer, size_t name_len,
                                         GLuint* tex_id_block);

/* Sould be called everytime the texture pack is changed */
void cub_block_free_all_textures();

#ifdef CUB_DEV_STAGE
/* Use it to add new block to the binary file representing the list of blocks */
void cub_block_new_block(const char* name, cub_block_t id, cubBlockType type);
#endif

#endif
