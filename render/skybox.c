#include "render/skybox.h"

GLuint cub_skybox_load_cubemap()
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
            errx(1, "Skybox image not found avec %d",i);
            stbi_image_free(data);
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


void cub_skybox_setup_renderer(cubSkyboxRenderer* renderer) {
    printf("cub_skybox_setup_renderer: Starting renderer initialisation...\n");
    const GLfloat skybox_vertices[] = {
        // positions          
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
    GLuint buffers[1];
    glGenBuffers(1, buffers);
    glGenVertexArrays(1, &renderer->VAO);

    // Bind the vertex array object then set up the other buffers data
    glBindVertexArray(renderer->VAO);
    // 1) Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices),
            skybox_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
            (void*)0);

    renderer->cubemapTexture = cub_skybox_load_cubemap();

    renderer->VBO = buffers[0];
    renderer->shader_program = cub_utils_build_shader("skybox");
    
    glUseProgram(renderer->shader_program);
    
    renderer->view_uni_loc = glGetUniformLocation(renderer->shader_program, "view");
    renderer->projection_uni_loc = glGetUniformLocation(renderer->shader_program, "projection");
    glUniform1i(glGetUniformLocation(renderer->shader_program, "skybox"), 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cub_skybox_free_renderer(cubSkyboxRenderer* renderer)
{
    glDeleteBuffers(1, &renderer->VBO);
    glDeleteVertexArrays(1, &renderer->VAO);
    glDeleteProgram(renderer->shader_program);

}
