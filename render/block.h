#ifndef CUB_RENDER_BLOCK_H
#define CUB_RENDER_BLOCK_H

#include "utils/utils.h"
#include "utils/open_gl.h"
#include "utils/shader.h"

#define BLOCK_LIST_FILE "block-list.cubic"
#define CUB_MAX_BLOCK_STRLEN 16

// Define how the block must be textured
typedef enum cubBlockTextureType {
    CUBE_UNIQUE_FACES,
    CUBE_TOP_BOTTOM,
    CUBE_SAME_FACES
} cubBlockTextureType;

typedef struct cubBlockData {
    cubBlockTextureType type;
    // bool is_solid; -> later
    char name[CUB_MAX_BLOCK_STRLEN];
    cub_block_t id;
    // 1) If type = CUBE_SAME_FACES: textures_id[0] = texId
    // 2) If type = CUBE_TOP_BOTTOM:
    //      - textures_id[0] = texTopId
    //      - textures_id[1] = texBottomId
    //      - textures_id[2] = texNSEWId
    // 3) If type = CUBE_UNIQUE_FACES: order is: S E N W T B
    GLuint* textures_id;
} cubBlockData;

typedef struct cubBlockList {
    // Array of all blocks
    cubBlockData* blocks;
    // Total number of blocks
    size_t nb_blocks;
} cubBlockList;

typedef struct cubBlockRenderer {
    GLuint VAO; // Vertex Array Object
    GLuint VBO, EBO; // Vertex BUffer & Element Buffer Objects
    GLuint shader_program;
    // Order: South(0) - East(1) - North(2) - West(3) - Top(4) - Bottom(5)
    GLint tex_locations[6];
    GLint model_uni_loc; // Uniform location of model_matrix
    cubMat4 model_matrix;
    cubBlockList block_list;
} cubBlockRenderer;

// -------- Rendering functions --------
/* Should be called only once to set up the block renderer */
void cub_block_setup_renderer(cubBlockRenderer* renderer);

/* When calling this, it is assumed that the correct shader program
 * has been bound before the call ! */
void cub_block_render(cubBlockRenderer* renderer, cub_block_t id,
                        cubVec3 position);

/* Should be called only at the end of the game */
void cub_block_free_renderer(cubBlockRenderer* renderer);

// -------- Loading functions --------
/* Sould be called only once, at game launch to initialise block_list */
void cub_block_load_block_list(cubBlockList* block_list);

/* Loads a texture pack */
void cub_block_load_texture_pack(cubBlockList* block_list);

void cub_block_load_texture_same_faces(char* buffer, size_t name_len,
                                        GLuint* tex_id_block);
void cub_block_load_texture_top_bottom_faces(char* buffer, size_t name_len,
                                             GLuint* tex_id_block);
void cub_block_load_texture_unique_faces(char* buffer, size_t name_len,
                                         GLuint* tex_id_block);

/* Sould be called everytime the texture pack is changed */
void cub_block_free_all_textures(cubBlockList* block_list);

#ifdef CUB_DEV_STAGE
/* Use it to add new block to the binary file representing the list of blocks */
void cub_block_new_block(const char* name, cub_block_t id,
                            cubBlockTextureType type);
#endif

#endif
