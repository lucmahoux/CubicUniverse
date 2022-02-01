#ifndef CUB_GRAPHICS_OPEN_GL_H
#define CUB_GRAPHICS_OPEN_GL_H

#include <GL/glew.h> // BEFORE EVERYTHING -> retrieve headers from drivers
#include <GLFW/glfw3.h>
#include "utils/utils.h"
#include "utils/matrix.h"
#include "utils/vector.h"

/* Load the texture from the texture folder - check for errors */
GLchar* cub_graphics_load_texture(GLint* width, GLint* height,
                                    GLint* nb_channels, const char* img_name);

#endif
