#include "utils/shader.h"

void get_shader_source(char* file, ShaderSource* shader) {
    printf("Searching for shader: %s\n", file);
    FILE* fp;
    if ( (fp = fopen(file, "r")) == NULL ) {
        shader->source = NULL;
        shader->shader_type = NONE;
    } else {
        char* source = malloc(BUFFER_SIZE + 1);
        if (!source)
            errx(1, "get_shader_source: malloc failed!\n");
        size_t nb_read, total_bytes = 0;
        char buffer[BUFFER_SIZE];
        while ((nb_read = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) != 0) {
            memcpy((void*)(source + total_bytes), buffer, nb_read);
            total_bytes += nb_read;
            if (nb_read >= BUFFER_SIZE) {
                source = realloc(source, total_bytes + BUFFER_SIZE + 1);
                if (!source)
                    errx(1, "get_shader_source: realloc failed!\n");
            }
        }
        *(char*)(source + total_bytes) = '\0';
        shader->source = source;
        if (fclose(fp))
            err(1, NULL);
    }
}

ShaderSource* get_shaders(char* path, size_t path_len, uint8_t* len) {
    ShaderSource* shader_sources = malloc(3 * sizeof(ShaderSource));
    if (!shader_sources)
        errx(1, "get_shaders: malloc failed!");

    char file[path_len + 10];
    char* type = &file[path_len + 1];
    memcpy(file, path, path_len);
    memcpy(&file[path_len], "_?.shader\0", 10);

    // Vertex Shader
    *type = 'V';
    shader_sources[0].shader_type = VERTEX_SHADER;
    get_shader_source(file, &shader_sources[0]);
    // Geometry Shader
    *type = 'G';
    shader_sources[1].shader_type = GEOMETRY_SHADER;
    get_shader_source(file, &shader_sources[1]);
    // Fragment Shader
    *type = 'F';
    shader_sources[2].shader_type = FRAGMENT_SHADER;
    get_shader_source(file, &shader_sources[2]);

    *len = 3;
    return shader_sources;
}

void create_compile_shader(ShaderSource* shader, const char* name) {
    // Create the shader
    switch (shader->shader_type) {
        case VERTEX_SHADER:
            shader->id = glCreateShader(GL_VERTEX_SHADER);
            break;
        case GEOMETRY_SHADER:
            shader->id = glCreateShader(GL_GEOMETRY_SHADER);
            break;
        case FRAGMENT_SHADER:
            shader->id = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        default:
            errx(1, "create_compile_shader: unknown shader type!");
    }

    // Compile it & check for errors
    glShaderSource(shader->id, 1, (const GLchar**)(&shader->source), NULL);
    glCompileShader(shader->id);
    GLint success;
    glGetShaderiv(shader->id, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint info_len;
        glGetShaderiv(shader->id, GL_INFO_LOG_LENGTH, &info_len);
        GLchar infoLog[info_len];
        glGetShaderInfoLog(shader->id, info_len, &info_len, infoLog);
        if (write(STDOUT_FILENO, infoLog, info_len) == -1)
            err(1, NULL);
        errx(1, "Compilation failed: shader_name =  %s & shader_type = %i!",
                name, shader->shader_type);
    }
}

GLuint build_shader(const char* shader_name) {
    size_t path_len;
    char* path = utils_strconcat(SHADERS_PATH, shader_name, &path_len);
    uint8_t len;
    ShaderSource* shader_srcs = get_shaders(path, path_len, &len);
    free(path);

    GLuint shaderProgram = glCreateProgram();
    uint8_t nb_shaders = 0;
    for (uint8_t i = 0; i < len; ++i) {
        ShaderSource* shader = &shader_srcs[i];
        if (shader->shader_type != NONE) {
            create_compile_shader(shader, shader_name);
            glAttachShader(shaderProgram, shader->id);
            ++nb_shaders;
        }
    }
    if (nb_shaders == 0)
        errx(1, "build_shader: no shader type was found!");

    glLinkProgram(shaderProgram);
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLint info_len;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &info_len);
        GLchar infoLog[info_len];
        glGetProgramInfoLog(shaderProgram, info_len, &info_len, infoLog);
        if (write(STDOUT_FILENO, infoLog, info_len) == -1)
            err(1, NULL);
        errx(1, "Program linking failed: shader_name = %s!", shader_name);
    }

    for (uint8_t i = 0; i < len; ++i) {
        ShaderSource* shader = &shader_srcs[i];
        if (shader->shader_type != NONE) {
            glDetachShader(shaderProgram, shader->id);
            glDeleteShader(shader->id);
            free(shader->source);
        }
    }

    free(shader_srcs);
    return shaderProgram;
}
