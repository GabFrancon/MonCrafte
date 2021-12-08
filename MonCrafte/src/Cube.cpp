#include "Cube.h"

Cube::Cube()
{
    vertexPositions = {
        // left
        -0.5f,-0.5f,-0.5f, 
        -0.5f,-0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
        // right
         0.5f, 0.5f, 0.5f,
         0.5f,-0.5f,-0.5f,
         0.5f, 0.5f,-0.5f,
         0.5f, 0.5f, 0.5f,
         0.5f,-0.5f, 0.5f,
         0.5f,-0.5f,-0.5f,
        // bottom
         0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,
         0.5f,-0.5f,-0.5f,
         0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,
        // top
         0.5f, 0.5f, 0.5f,
         0.5f, 0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f,
         0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
        -0.5f, 0.5f, 0.5f,
        // back
         0.5f, 0.5f,-0.5f, 
        -0.5f,-0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f,
         0.5f, 0.5f,-0.5f,
         0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        // front
        -0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,
         0.5f,-0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
         0.5f,-0.5f, 0.5f,
         0.5f, 0.5f, 0.5f
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
}

void Cube::bindToGPU()
{
    // generate the VAO
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

    // deactivate the VAO for now
    glBindVertexArray(0);
}

void Cube::drawGeometry(std::map<std::string, bool> facesRendering)
{
    glBindVertexArray(vao);
    if(facesRendering["left"])
        glDrawArrays(GL_TRIANGLES, 0, 6);
    if (facesRendering["right"])
        glDrawArrays(GL_TRIANGLES, 6, 6);
    if (facesRendering["bottom"])
        glDrawArrays(GL_TRIANGLES, 12, 6);
    if (facesRendering["top"])
        glDrawArrays(GL_TRIANGLES, 18, 6);
    if (facesRendering["back"])
        glDrawArrays(GL_TRIANGLES, 24, 6);
    if (facesRendering["front"])
        glDrawArrays(GL_TRIANGLES, 30, 6);
}
