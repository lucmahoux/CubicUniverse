#ifndef CUB_UTILS_UTILS_H
#define CUB_UTILS_UTILS_H
#define cub_unused __attribute__((unused))

#include <err.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// OpenGL headers
#include <GL/glew.h> // -> Before (headers included from drivers)
#include <GLFW/glfw3.h>

// Linear Algebra
#include "matrix.h"
#include "vector.h"

extern char ASSETS_PATH[];
extern char SHADERS_PATH[];
extern char TEXTURES_PATH[];
extern char GAMEFILES_PATH[];

#define cub_block_t uint32_t
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

typedef struct cubCamera {
    GLint view_uni_loc; // Uniform location of view matrix
    cubMat4 view_matrix;
    GLint projection_uni_loc; // Uniform location of projection matrix
    cubMat4 projection_matrix;
    float fov; // Field Of View
    float aspect_ratio; // Aspect ration = width / height
    float near, far; // Near and Far planes distances
} cubCamera;

typedef struct cubGame {
    GLFWwindow* window;
    cubBlockRenderer block_renderer;
    cubCamera camera;
} cubGame;

/* Concatenate two strings and return the total length (excluding '\0') */
char* cub_utils_strconcat(const char* s1, const char* s2, size_t* len);

#endif
