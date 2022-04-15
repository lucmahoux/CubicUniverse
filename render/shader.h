#ifndef CUB_RENDER_SHADER_H
#define CUB_RENDER_SHADER_H

#include "utils/utils.h"

#define BUFFER_SIZE 512

typedef enum cubShaderTypeEnum {
    VERTEX_SHADER,
    FRAGMENT_SHADER,
    GEOMETRY_SHADER,
    NONE
} ShaderTypeEnum;

typedef struct cubShaderSource {
    char* source;
    ShaderTypeEnum shader_type;
    GLuint id;
} ShaderSource;

/* Link & compile all the shaders identified by 'const char* name'
 * and return the shader program ID */
GLuint build_shader(const char* shader_name);

#endif
