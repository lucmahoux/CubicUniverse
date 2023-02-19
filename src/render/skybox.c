#include "skybox.h"

static GLuint skybox_load_cubemap(void)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    stbi_set_flip_vertically_on_load(0);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++)
    {
        char* face;
        if(i == 0)
            face = "../assets/skybox/right.jpg";
        else if(i == 1)
            face = "../assets/skybox/left.jpg";
        else if(i == 2)
            face = "../assets/skybox/top.jpg";
        else if(i == 3)
            face = "../assets/skybox/bottom.jpg";
        else if(i == 4)
            face = "../assets/skybox/front.jpg";
        else
            face = "../assets/skybox/back.jpg";
        unsigned char *data = stbi_load(face, &width, &height,
                &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                    data);
            stbi_image_free(data);
        }
        else
        {
            stbi_image_free(data);
            errx(1, "Skybox image not found with %d",i);
        }
    }
    stbi_set_flip_vertically_on_load(1);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void skybox_setup_renderer(SkyboxRenderer* renderer)
{
    printf("skybox_setup_renderer: Starting renderer initialisation...\n");
    const GLfloat skybox_vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // Generate the buffers
    setup_render_buffer_object(&renderer->RBO);
    glGenBuffers(1, &renderer->RBO.VBO);
    glGenVertexArrays(1, &renderer->RBO.VAO);

    // Bind the vertex array object then set up the other buffers data
    glBindVertexArray(renderer->RBO.VAO);
    // 1) Vertex Buffer
    renderer->RBO.nb_elements = 36;
    glBindBuffer(GL_ARRAY_BUFFER, renderer->RBO.VBO);
    glBufferData(GL_ARRAY_BUFFER, renderer->RBO.nb_elements * 3
                 * sizeof(GLfloat), skybox_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (GLvoid*) 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    renderer->cubemapTexture = skybox_load_cubemap();
    renderer->shader_program = build_shader("skybox");
    glUseProgram(renderer->shader_program);
    try_get_uniform_location(renderer->shader_program, &renderer->VP_matrix_loc,
                             "VP_matrix", "skybox_setup_renderer");
}

void skybox_render(SkyboxRenderer* renderer, Camera* camera)
{
    // Prepare skybox rendering
    glDepthFunc(GL_LEQUAL);
    glUseProgram(renderer->shader_program);
    // Remove the translation part of the view matrix
    mat4 VP_matrix = camera->view_matrix;
    VP_matrix.coefficients[12] = 0.0f;
    VP_matrix.coefficients[13] = 0.0f;
    VP_matrix.coefficients[14] = 0.0f;
    // Update the VP_matrix uniform -> VP = projection * view
    VP_matrix = mat4_product_simd(camera->projection_matrix, VP_matrix);
    glUniformMatrix4fv(renderer->VP_matrix_loc, 1, GL_FALSE, VP_matrix.coefficients);

    // Render skybox
    glBindVertexArray(renderer->RBO.VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, renderer->RBO.nb_elements);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void skybox_free_renderer(SkyboxRenderer* renderer)
{
    free_render_buffer_object(&renderer->RBO);
    glDeleteProgram(renderer->shader_program);
}
