#include "Block.h"

void Block::initGeometry()
{
    vertexPositions = {
        // left
        -1.0f,-1.0f,-1.0f, 
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        // right
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
         1.0f,-1.0f,-1.0f,
        // bottom
         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        // top
         1.0f, 1.0f, 1.0f,
         1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        // back
         1.0f, 1.0f,-1.0f, 
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        // front
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f
    };

    vertexNormals = {
        // left
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        // right 
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
        // bottom
         0.0f,-1.0f, 0.0f,
         0.0f,-1.0f, 0.0f,
         0.0f,-1.0f, 0.0f,
         0.0f,-1.0f, 0.0f,
         0.0f,-1.0f, 0.0f,
         0.0f,-1.0f, 0.0f,
        // top
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
        // back
         0.0f, 0.0f,-1.0f,
         0.0f, 0.0f,-1.0f,
         0.0f, 0.0f,-1.0f,
         0.0f, 0.0f,-1.0f,
         0.0f, 0.0f,-1.0f,
         0.0f, 0.0f,-1.0f,
        // front
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f
    };

    vertexTextures = {
        // left
         0.0f, 0.0f, 
         1.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 1.0f,
        // right
         1.0f, 1.0f,
         0.0f, 0.0f,
         0.0f, 1.0f,
         1.0f, 1.0f,
         1.0f, 0.0f,
         0.0f, 0.0f,
        // bottom
         1.0f, 1.0f,
         0.0f, 0.0f,
         1.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 1.0f,
         0.0f, 0.0f,
        // top
         1.0f, 1.0f,
         1.0f, 0.0f,
         0.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 0.0f,
         0.0f, 1.0f,
        // back
         1.0f, 1.0f ,
         0.0f, 0.0f,
         0.0f, 1.0f,
         1.0f, 1.0f,
         1.0f, 0.0f,
         0.0f, 0.0f,
        // front
         0.0f, 1.0f,
         0.0f, 0.0f,
         1.0f, 0.0f,
         0.0f, 1.0f,
         1.0f, 0.0f,
         1.0f, 1.0f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // position coordinates
    size_t vertexBufferSize = sizeof(float) * vertexPositions.size();
    glGenBuffers(1, &posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, posVbo);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertexPositions.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal coordinates
    size_t normalBufferSize = sizeof(float) * vertexNormals.size();
    glGenBuffers(1, &norVbo);
    glBindBuffer(GL_ARRAY_BUFFER, norVbo);
    glBufferData(GL_ARRAY_BUFFER, normalBufferSize, vertexNormals.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // texture coordinates
    size_t textureBufferSize = sizeof(float) * vertexTextures.size();
    glGenBuffers(1, &texVbo);
    glBindBuffer(GL_ARRAY_BUFFER, texVbo);
    glBufferData(GL_ARRAY_BUFFER, textureBufferSize, vertexTextures.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Block::render(const GLuint program, glm::mat4 transMat, const GLuint texture)
{
    // send uniform values to shaders
    glUniformMatrix4fv(glGetUniformLocation(program, "transMat"), 1, GL_FALSE, glm::value_ptr(transMat));
    glUniform1f(glGetUniformLocation(program, "ambient"), ambient);
    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // draw 3D model
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Block::freeBuffer()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &posVbo);
    glDeleteBuffers(1, &norVbo);
    glDeleteBuffers(1, &texVbo);
}
