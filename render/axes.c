#include "render/axes.h"

void axes_setup_renderer(AxesRenderer* renderer) {
    printf("axes_setup_renderer: Starting renderer initialisation...\n");
    const GLfloat axes_VBO[] = {
        0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   0.0f,
        0.1f,   0.0f,   0.0f,   1.0f,   0.0f,   0.0f,

        0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   0.0f,
        0.0f,   0.1f,   0.0f,   0.0f,   1.0f,   0.0f,

        0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,
        0.0f,   0.0f,   0.1f,   0.0f,   0.0f,   1.0f
    };

    setup_render_buffer_object(&renderer->RBO);
    glGenBuffers(1, &renderer->RBO.VBO);
    glGenVertexArrays(1, &renderer->RBO.VAO);

    glBindVertexArray(renderer->RBO.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->RBO.VBO);
    renderer->RBO.nb_elements = 6;
    glBufferData(GL_ARRAY_BUFFER, renderer->RBO.nb_elements * 6
                 * sizeof(GLfloat), axes_VBO, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(GLfloat), (GLvoid*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    renderer->shader_program = build_shader("axes");

    glUseProgram(renderer->shader_program);
    try_get_uniform_location(renderer->shader_program, &renderer->VP_matrix_loc,
                             "VP_matrix", "axes_setup_renderer");
}

void axes_render(AxesRenderer* renderer, Camera* camera)
{
    // Prepare axes rendering
    glUseProgram(renderer->shader_program);
    // Remove the translation part of the view matrix
    mat4 VP_matrix = camera->view_matrix;
    VP_matrix.coeffs[12] = 0.0f;
    VP_matrix.coeffs[13] = 0.0f;
    VP_matrix.coeffs[14] = 0.0f;
    // Update the VP_matrix uniform -> VP = projection * view
    VP_matrix = mat4_product_simd(camera->projection_matrix, VP_matrix);
    glUniformMatrix4fv(renderer->VP_matrix_loc, 1, GL_FALSE, VP_matrix.coeffs);

    // Render axes
    glBindVertexArray(renderer->RBO.VAO);
    glDrawArrays(GL_LINES, 0, renderer->RBO.nb_elements);
    glBindVertexArray(0);
}

void axes_free_renderer(AxesRenderer* renderer)
{
    free_render_buffer_object(&renderer->RBO);
    glDeleteProgram(renderer->shader_program);
}
