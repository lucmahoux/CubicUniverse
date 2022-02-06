#include "utils/window.h"

void cub_utils_framebuffer_size_callback(cub_unused GLFWwindow* window,
                                            int width, int height) {
    // Update OpenGL viewport size
    glViewport(0, 0, width, height);
}

void cub_utils_setup_window(GLFWwindow** window, int width, int height,
                            const char* window_name) {
    // Initialise GLFW
    if (!glfwInit())
        errx(1, "Failed to initialise GLFW\n");

    // Window options
    // 4x antialiasing
    glfwWindowHint(GLFW_SAMPLES, 4);
    // Using OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Using OpenGL core profile = modern OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // For MacOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    *window = glfwCreateWindow(width, height, window_name, NULL, NULL);
    if (*window == NULL) {
        glfwTerminate();
        errx(1, "Couldn't open a GLFW window!");
    }

    // Callback when the window is resized
    glfwSetFramebufferSizeCallback(*window, cub_utils_framebuffer_size_callback);

    // Set up GLFW's window input mode
    // Cursor hidden & cursor can't leave the window (3D camera)
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwGetKey returns GLFW_PRESS only if the key is currently pressed
    glfwSetInputMode(*window, GLFW_STICKY_KEYS, GLFW_FALSE);
    // glfwGetKey will return GLFW_PRESS the next call
    // even if it has been released since then
    glfwSetInputMode(*window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    // Disables Caps: Lock & Num Lock keys
    glfwSetInputMode(*window, GLFW_LOCK_KEY_MODS, GLFW_FALSE);

    // Set the window to the current context
    glfwMakeContextCurrent(*window);

    // Initialise GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
        errx(1, "Failed to initialise GLEW!");
}

void cub_utils_start_window_loop(GLFWwindow* window,
                                 window_callback clear_screen_handler,
                                 window_callback input_handler,
                                 window_callback renderer_handler,
                                 window_callback close_handler,
                                 void* handlers_param) {
    while (!glfwWindowShouldClose(window)) {
        input_handler(handlers_param);
        clear_screen_handler(handlers_param);
        renderer_handler(handlers_param);
        // Swap buffers - double buffer rendering
        glfwSwapBuffers(window);
        // Gather all the events
        glfwPollEvents();
    }

    close_handler(handlers_param);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void cub_utils_close_window(GLFWwindow* window) {
    glfwSetWindowShouldClose(window, true);
}
