#include "render/skybox.h"

void cub_skybox_load_cubemap()
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++)
    {
        char* face;
        if(i == 0)
            face = "right.jpg";
        else if(i == 1)
            face = "left.jpg";
        else if(i == 2)
            face = "top.jpg";
        else if(i == 3)
            face = "bottom.jpg";
        else if(i == 4)
            face = "front.jpg";
        else
            face = "back.jpg";
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
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void cub_skybox_setup_renderer(cubSkyboxRenderer* renderer) {
    printf("cub_skybox_setup_renderer: Starting renderer initialisation...\n");
    const GLfloat block_vertices[] = {
        0.0f,   0.0f,   1.0f,   0.0f,   0.0f,   1.0f,   // Face 1 - South
        1.0f,   0.0f,   1.0f,   1.0f,   0.0f,   1.0f,
        1.0f,   1.0f,   1.0f,   1.0f,   1.0f,   1.0f,
        0.0f,   1.0f,   1.0f,   0.0f,   1.0f,   1.0f,

        1.0f,   0.0f,   1.0f,   0.0f,   0.0f,   2.0f,   // Face 2 - East
        1.0f,   0.0f,   0.0f,   1.0f,   0.0f,   2.0f,
        1.0f,   1.0f,   0.0f,   1.0f,   1.0f,   2.0f,
        1.0f,   1.0f,   1.0f,   0.0f,   1.0f,   2.0f,

        1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   3.0f,   // Face 3 - North
        0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   3.0f,
        0.0f,   1.0f,   0.0f,   1.0f,   1.0f,   3.0f,
        1.0f,   1.0f,   0.0f,   0.0f,   1.0f,   3.0f,

        0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   // Face 4 - West
        0.0f,   0.0f,   1.0f,   1.0f,   0.0f,   4.0f,
        0.0f,   1.0f,   1.0f,   1.0f,   1.0f,   4.0f,
        0.0f,   1.0f,   0.0f,   0.0f,   1.0f,   4.0f,

        0.0f,   1.0f,   1.0f,   0.0f,   0.0f,   5.0f,   // Face 5 - Top
        1.0f,   1.0f,   1.0f,   1.0f,   0.0f,   5.0f,
        1.0f,   1.0f,   0.0f,   1.0f,   1.0f,   5.0f,
        0.0f,   1.0f,   0.0f,   0.0f,   1.0f,   5.0f,

        0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   6.0f,   // Face 6 - Bottom
        1.0f,   0.0f,   0.0f,   1.0f,   0.0f,   6.0f,
        1.0f,   0.0f,   1.0f,   1.0f,   1.0f,   6.0f,
        0.0f,   0.0f,   1.0f,   0.0f,   1.0f,   6.0f
    };
    
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
    GLuint buffers[2];
    GLuint VAO;
    glGenBuffers(2, buffers);
    glGenVertexArrays(1, &VAO);

    // Bind the vertex array object then set up the other buffers data
    glBindVertexArray(VAO);
    // 1) Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices),
            skybox_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
            (void*)0);

    GLuint cubemapTexture = cub_skybox_load_cubemap();

    renderer->shader_program = cub_utils_build_shader("skybox");

}

void cub_skybox_render(cubSkyboxRenderer* renderer)
{
    glDeptechFunc(GL_LEQUAL);
    renderer->shader_program.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default

}
