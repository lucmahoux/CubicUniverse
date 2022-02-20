#ifndef CUB_UTILS_SHADER_H
#define CUB_UTILS_SHADER_H

#include "utils/utils.h"

#define BUFFER_SIZE 512

typedef enum ShaderTypeEnum {
    VERTEX_SHADER,
    FRAGMENT_SHADER,
    GEOMETRY_SHADER,
    NONE
} ShaderTypeEnum;

typedef struct ShaderSource {
    char* source;
    ShaderTypeEnum shader_type;
    GLuint id;
} ShaderSource;

/* Try to retrieve the source code of the shader if the shader file exists.
 * If not shader->shader_type is set to NONE.
 * Otherwise, only shader->source is set to an allocated memory area
 * representing the source code of the shader. */
void get_shader_source(char* file, ShaderSource* shader);

/* Retrieve all possible shader types with their associated source code.
 *  - const char* name: the shader identifier
 *  - uint8_t* len: the number of shaders in ShaderSource* returned array */
ShaderSource* get_shaders(char* path, size_t path_len, uint8_t* len);

/* Create the correct shader type & compile it while checking for errors */
void create_compile_shader(ShaderSource* shader, const char* name);

/* Link & compile all the shaders identified by 'const char* name'
 * and return the shader program ID */
GLuint build_shader(const char* shader_name);

#endif
