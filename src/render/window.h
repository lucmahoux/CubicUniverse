#ifndef CUB_RENDER_WINDOW_H
#define CUB_RENDER_WINDOW_H

#include "utils.h"

typedef void (*window_callback) (void*);

/* To handle window resizing -> OpenGL viewport update */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


/* Should be called once to initialise the window */
void setup_window(GLFWwindow** window, int width, int height,
                  const char* window_name);

/* Starts the window loop and call the different handlers with 'handlers_param'
 * as a parameter (usually the cubGame struct) */
void start_window_loop(GLFWwindow* window,
                       window_callback clear_screen_handler,
                       window_callback input_handler,
                       window_callback renderer_handler,
                       window_callback close_handler,
                       void* handlers_param);

/* Respectfully ask the window to close :) */
void close_window(GLFWwindow* window);

#endif
