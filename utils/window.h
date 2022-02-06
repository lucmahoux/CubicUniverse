#ifndef CUB_UTILS_WINDOW_H
#define CUB_UTILS_WINDOW_H

#include "utils/utils.h"

typedef void (*window_callback) (void*);

/* To handle window resizing -> OpenGL viewport update */
void cub_utils_framebuffer_size_callback(cub_unused GLFWwindow* window,
                                            int width, int height);

/* Should be called once to initialise the window */
void cub_utils_setup_window(GLFWwindow** window, int width, int height,
                            const char* window_name);

/* Starts the window loop and call the different handlers with 'handlers_param'
 * as a parameter (usually the cubGame struct) */
void cub_utils_start_window_loop(GLFWwindow* window,
                                 window_callback clear_screen_handler,
                                 window_callback input_handler,
                                 window_callback renderer_handler,
                                 window_callback close_handler,
                                 void* handlers_param);

/* Respectfully ask the window to close :) */
void cub_utils_close_window(GLFWwindow* window);

#endif
