#include "line_renderer.h"

void line_renderer_setup(LineRenderer* renderer) {
    printf("line_renderer_setup: Starting renderer initialisation...\n");

    setup_render_buffer_object(&renderer->RBO);
    glGenBuffers(1, &renderer->RBO.VBO);
    glGenVertexArrays(1, &renderer->RBO.VAO);

    glBindVertexArray(renderer->RBO.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->RBO.VBO);
    renderer->RBO.nb_elements = 2;
    glBufferData(GL_ARRAY_BUFFER, renderer->RBO.nb_elements * 6
                 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(GLfloat), (GLvoid*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    renderer->shader_program = build_shader("lines");

    glUseProgram(renderer->shader_program);
    try_get_uniform_location(renderer->shader_program, &renderer->VP_matrix_loc,
                             "VP_matrix", "line_renderer_setup");
}

void line_render(LineRenderer* renderer, vec3 start, vec3 end, vec3 color)
{
    glUseProgram(renderer->shader_program);
    GLfloat vertex_data[] = {
        start.coords[0], start.coords[1], start.coords[2],
        color.coords[0], color.coords[1], color.coords[2],
        end.coords[0], end.coords[1], end.coords[2],
        color.coords[0], color.coords[1], color.coords[2]
    };
    glBindBuffer(GL_ARRAY_BUFFER, renderer->RBO.VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_data), vertex_data);
    glBindVertexArray(renderer->RBO.VAO);
    glDrawArrays(GL_LINES, 0, renderer->RBO.nb_elements);
    glBindVertexArray(0);
}

void line_renderer_free(LineRenderer* renderer)
{
    free_render_buffer_object(&renderer->RBO);
    glDeleteProgram(renderer->shader_program);
}
