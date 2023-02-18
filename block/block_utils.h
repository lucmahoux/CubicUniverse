#ifndef CUB_BLOCK_BLOCK_UTILS_H
#define CUB_BLOCK_BLOCK_UTILS_H

#include "physics/physics_utils.h"
#include "render/open_gl.h"
#include "render/texture_atlas.h"
#include "utils/utils.h"

// BlockData Unique ID
#define BDuid uint16_t
// BlockState Unique ID
#define BSuid uint32_t
#define bs_key uint8_t
#define bs_val uint8_t
#define MAX_BLOCK_STRLEN 32
#define AIR_ID 0
#define BLOCK_LIST_FILE "block-list.cubic"
/* If the blockstate texturer exists, then its ID in the 'bs_keys' of a
 * cubBlockData must be 0! */
#define BS_TEX_ID 0

// Shader-related macros
#define SHADER_CUSTOM_TEXTURE_DATA_SIZE 5
#define SHADER_CUSTOM_PACKED_DATA_SIZE 1
#define SHADER_CUSTOM_TOTAL_UINTS SHADER_CUSTOM_TEXTURE_DATA_SIZE\
                                  + SHADER_CUSTOM_PACKED_DATA_SIZE
#define SHADER_CUBIC_PACKED_DATA_SIZE 2
#define SHADER_CUBIC_TOTAL_UINTS SHADER_CUBIC_PACKED_DATA_SIZE
#define CUBIC_VBO_BUFFER_SIZE 4096

typedef enum cubBlockStateKey {
    BS_FACING,  // Orientation of the block
    BS_AGE,     // Age longevity of a crop
    BS_LIT,     // When a block can be "turned on/off" (e.g. furnace)
    BS_COLOR,   // color suffixes for all textures
    BS_HALF,    // For objects composed of 2 blocks (e.g. a door, double plant)
    BS_FLOWER_TYPE,
    BS_DOUBLE_PLANT_TYPE,
    BS_WOOD_TYPE
} BlockStateKey;

typedef enum cubBlockStateValue {
    // BS_FACING
    BSV_NORTH = 0, BSV_SOUTH, BSV_EAST, BSV_WEST, BSV_FACING_LEN,
    // BS_AGE
    BSV_STAGE0 = 0, BSV_STAGE1, BSV_STAGE2, BSV_STAGE3, BSV_AGE_LEN,
    // BS_LIT
    BSV_FALSE = 0, BSV_TRUE, BSV_LIT_LEN,
    // BS_COLOR
    BSV_WHITE = 0, BSV_ORANGE, BSV_MAGENTA, BSV_LIGHT_BLUE, BSV_YELLOW,
    BSV_LIME, BSV_PINK, BSV_GRAY, BSV_LIGHT_GRAY, BSV_CYAN, BSV_PURPLE,
    BSV_BLUE, BSV_BROWN, BSV_GREEN, BSV_RED, BSV_BLACK, BSV_COLOR_LEN,
    // BS_HALF
    BSV_LOWER = 0, BSV_UPPER, BSV_HALF_LEN,
    // BS_FLOWER_TYPE
    BSV_POPPY = 0, BSV_ORCHID, BSV_ALLIUM, BSV_AZURE_BLUET, BSV_TULIP_RED,
    BSV_TULIP_ORANGE, BSV_TULIP_WHITE, BSV_TULIP_PINK, BSV_OXEYE,
    BSV_CORNFLOWER, BSV_LILY_OF_THE_VALLEY, BSV_FLOWER_TYPE_LEN,
    // BS_DOUBLE_PLANT_TYPE
    BSV_SUNFLOWER = 0, BSV_LILAC, BSV_GRASS, BSV_FERN, BSV_ROSE, BSV_PEONY,
    BSV_DOUBLE_PLANT_TYPE_LEN,
    // BS_WOOD_TYPE
    BSV_ACACIA = 0, BSV_BIRCH, BSV_DARK_OAK, BSV_JUNGLE, BSV_OAK, BSV_SPRUCE,
    BSV_WOOD_TYPE_LEN
} BlockStateValue;

extern const uint8_t CUB_BSV_LEN[];

typedef char* (*BS_parser) (BlockStateValue, char*);

// Define how the textures file names must be formed.
// It is also used to bind the correct textures when using the default VAO
typedef enum cubRenderType {
    RT_NONE             =   0x00,   // no texture (air)
    RT_TOP              =   0x01,   // _top suffix
    RT_BOTTOM           =   0x02,   // _bottom suffix
    RT_FRONT            =   0x04,   // _front suffix
    RT_BACK             =   0x08,   // _back suffix
    RT_LEFT             =   0x10,   // _left suffix
    RT_RIGHT            =   0x20,   // _right suffix
    RT_SIDE             =   0x40,   // _side suffix
    RT_DEFAULT          =   0x80    // no suffix
} RenderType;

typedef struct cubBlockFlags {
    // Does the player collide with the block?
    uint8_t is_solid        :   1;
    // Can it stand in the air without support?
    uint8_t has_gravity     :   1;
    // Does it stop light?
    uint8_t is_opaque       :   1;
    // Is it a cube?
    uint8_t is_cubic        :   1;
    // Does it have a blockstate modifying its textures ("bs texturer")?
    uint8_t has_bs_tex      :   1;
    // Only if has_bs_tex is TRUE, Is this BS a block CREATOR (!= MODIFICATOR)?
    uint8_t is_bs_creator   :   1;
    // Does it have BSs? (including the bs_name_mod which must be the first)
    uint8_t has_states      :   1;
    uint8_t unused          :   1;
} BlockFlags;

struct cubCustomBlockInfoRBO {
    GLsizei nb_elements; // Nbr of elements used to draw the block
    GLsizei element_offset; // Offset to the 1st element to draw
};

typedef struct cubBlockData {
    RenderType render_type;
    char name[MAX_BLOCK_STRLEN];
    BDuid id;
    BlockFlags flags;
    uint8_t nb_tex_draw; // Nbr of different textures to load on draw call
    uint16_t info_RBO_id; // Only for CUSTOM blocks
    // Set to NULL if the block does not have a BS name modifier.
    // Otherwise, point to the function used to parse its values.
    BS_parser bs_name_parser;
    TexAtlasQuad* textures;
    // Total number of states
    uint8_t nb_states;
    // The blockstate UID when all states are set to their lowest value (0)
    BSuid origin_BS_uid;
    bs_key* bs_keys;
    bs_val* bs_default_values;
} BlockData;

typedef struct cubBlockList {
    // Array of all blocks
    BlockData* blocks;
    // Total number of blocks
    BDuid nb_blocks;
} BlockList;

typedef enum cubBlockFaceID {
    FID_START,
    FID_TOP = FID_START,
    FID_BOTTOM,
    FID_SIDE_START,
    FID_FRONT = FID_SIDE_START,
    FID_BACK,
    FID_LEFT,
    FID_RIGHT,
    FID_SIDE_END = FID_RIGHT,
    FID_END = FID_RIGHT,
    FID_ENTRANCES
} FaceID;

#define FACE_BITMASK(faceID) (1 << faceID)

typedef struct cubBlockState {
    BlockData* block;
    BSuid BS_uid;
    bs_val* states;
} BlockState;

typedef struct cubBlockPaletteElement {
    BlockState BS;
    // Nbr of blocks referencing this palette element in the subchunk
    uint16_t nb_blocks;
} BP_elt;

struct cubCubicBlockShaderUniforms {
    GLint texture_atlas; // SAMPLER2D
    GLint texture_atlas_dim; // VEC2
    GLint texel_padding; // VEC2
    GLint VP_matrix; // MAT4
    GLint subchunk_pos; // IVEC3
};

struct cubCustomBlockShaderUniforms {
    GLint texture_atlas; // SAMPLER2D
    GLint texture_atlas_dim; // VEC2
    GLint VP_matrix; // MAT4
    GLint texture_data; // ARRAY OF 5 UINTS
    GLint packed_data;
    GLint subchunk_pos; // IVEC3
    GLint primary_rotations; // MAT4 * 3
    GLint secondary_rotations; // MAT4 * 8
};

typedef struct cubCustomBlockRenderer {
    GLuint shader; // Shader used to render non-cubic blocks
    // All the buffers used to render the blocks
    RenderBufferObject RBO;
    struct cubCustomBlockInfoRBO* RBO_chunks;
    struct cubCustomBlockShaderUniforms uniforms;
} CustomBlockRenderer;

typedef struct cubCubicBlockRenderer {
    GLuint shader; // Shader used to render face of cubic blocks
    RenderBufferObject RBO;
    struct cubCubicBlockShaderUniforms uniforms;
    GLuint cache[CUBIC_VBO_BUFFER_SIZE * SHADER_CUBIC_TOTAL_UINTS];
    size_t cache_offset;
} CubicBlockRenderer;

typedef struct cubBlockRenderer {
    struct cubCubicBlockRenderer cubic;
    struct cubCustomBlockRenderer custom;
    TextureAtlas texture_atlas;
} BlockRenderer;

// ---------- Useful Macros ----------- //
#define CUB_BSK_TEX(block_data) block_data->bs_keys[BS_TEX_ID]

/* Set the BlockState UID (BS->BS_uid field) of the given blockstate */
void block_set_BS_uid(BlockState* BS);

/* Creates a new BP_elt by allocating its memory.
 * The default number of blocks is set to 1! */
BP_elt* block_BP_elt_new(BlockState* BS);

/* Frees a block palette element */
void block_BP_elt_free(BP_elt* bp_elt);

#endif
