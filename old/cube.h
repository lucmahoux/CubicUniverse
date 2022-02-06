#ifndef CUB_GRAPHICS_CUBE_H
#define CUB_GRAPHICS_CUBE_H

#include "graphics/open_gl.h"
#include "graphics/shader.h"
#include "utils/stb_image.h"

void cub_graphics_framebuffer_size_callback(GLFWwindow* w,
                                            int width, int height);
void cub_graphics_input_handler(GLFWwindow* window);
int cub_graphics_open_window();

#endif
