#include "graphics/cube.h"

void cub_graphics_framebuffer_size_callback(cub_unused GLFWwindow* w,
                                            int width, int height) {
    glViewport(0, 0, width, height);
}

void cub_graphics_input_handler(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int cub_graphics_open_window() {
    // Initialise GLFW
    glewExperimental = true;
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialise GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Use OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Not the old OpenGL

    // Create the window
    GLFWwindow* window;
    window = glfwCreateWindow(1024, 768, "First Window", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    // Initialise GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initalize GLEW\n");
        return -1;
    }

    GLfloat  vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // Bind everything
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Unbind everything
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint cubeSP = cub_utils_build_shader("cube");

    // Generate the textures
    stbi_set_flip_vertically_on_load(true);
    GLuint t_grass, t_smiley;
    glGenTextures(1, &t_grass);
    glGenTextures(1, &t_smiley);
    // First texture -> grass
    glBindTexture(GL_TEXTURE_2D, t_grass);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLint width, height, nb_channels;
    GLchar* data = cub_graphics_load_texture(&width, &height,
                    &nb_channels, "grass.jpg");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                    GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(data);
    // Second texture -> smiley
    glBindTexture(GL_TEXTURE_2D, t_smiley);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = cub_graphics_load_texture(&width, &height,
                    &nb_channels, "smiley.png");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(data);
    // Set the uniforms accordingly
    glUseProgram(cubeSP);
    glUniform1i(glGetUniformLocation(cubeSP, "grass"), 0);
    glUniform1i(glGetUniformLocation(cubeSP, "smiley"), 1);
    glUseProgram(0);

    // Callback for when the window is resized
    glfwSetFramebufferSizeCallback(window,
        cub_graphics_framebuffer_size_callback);
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glEnable(GL_DEPTH_TEST); // Enable depth testing

    cub_vec3 cube_positions[] = {
        cub_utils_vec3(0.0f, 0.0f, 0.0f),
        cub_utils_vec3(2.0f, 5.0f, -15.0f),
        cub_utils_vec3(-1.5f, -2.2f, -2.5f),
        cub_utils_vec3(-3.8f, -2.0f, -12.3f),
        cub_utils_vec3(2.4f, -0.4f, -3.5f),
        cub_utils_vec3(-1.7f, 3.0f, -7.5f),
        cub_utils_vec3(1.3f, -2.0f, -2.5f),
        cub_utils_vec3(1.5f, 2.0f, -2.5f),
        cub_utils_vec3(1.5f, 0.2f, -1.5f),
        cub_utils_vec3(-1.3f, 1.0f, -1.5f)
    };

    glUseProgram(cubeSP);
    // Projection matrix
    cub_mat4 projection = cub_utils_mat4_perspective(RAD(45.0f),
                                 1024.0f / 768.0f, 0.1f, 100.0f);
    int projection_loc = glGetUniformLocation(cubeSP, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection.coeffs);
    glUseProgram(0);
    cub_vec3 rot_axis = CUB_VEC3_NORM(CUB_VEC3(1.0f, 0.3f, 0.5f));
    const float radius = 10.0f;

    while (!glfwWindowShouldClose(window)) {
        cub_graphics_input_handler(window);
        // Clear the screen with red
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // Clear the color buffer & the depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind the texture to textures units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t_grass);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, t_smiley);

        // Rendering
        glUseProgram(cubeSP);
        glBindVertexArray(VAO);


        // "Camera"
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        cub_mat4 view = cub_utils_mat4_view_matrix(
                    CUB_VEC3(camX, 0.0f, camZ),
                    CUB_VEC3(0.0f, 0.0f, 0.0f),
                    CUB_VEC3(0.0f, 1.0f, 0.0f));
        int view_loc = glGetUniformLocation(cubeSP, "view");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.coeffs);

        for (uint8_t i = 0; i < 10; ++i) {
            // Model matrix
            cub_mat4 model = CUB_MAT4_TRANS(CUB_MAT4(1.0f), cube_positions[i]);
            model = CUB_MAT4_ROT(model, glfwGetTime() * RAD(20.0f * i),
                                 rot_axis);
            int model_loc = glGetUniformLocation(cubeSP, "model");
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, model.coeffs);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // Swap buffers - double buffer rendering
        glfwSwapBuffers(window);
        // Gather all the events
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &t_grass);
    glDeleteTextures(1, &t_smiley);
    glDeleteProgram(cubeSP);

    glfwTerminate();
    return 0;
}
