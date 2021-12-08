#include "Sphere.h"

Sphere::Sphere()
{
    int accuracy = 50;
    float step = 1 / (float)accuracy;

    for (int i = 0; i < accuracy + 1; i++)
    {
        for (int j = 0; j < accuracy + 1; j++)
        {
            float theta = (float)i * step;
            float phi = (float)j * step;

            float x = cos(theta * 2 * M_PI) * sin(phi * M_PI);
            float y = sin(theta * 2 * M_PI) * sin(phi * M_PI);
            float z = cos(phi * M_PI);

            vertexPositions.push_back(x);
            vertexPositions.push_back(y);
            vertexPositions.push_back(z);

            vertexNormals.push_back(x);
            vertexNormals.push_back(y);
            vertexNormals.push_back(z);

            vertexTextures.push_back(theta);
            vertexTextures.push_back(phi);
        }
    }

    for (int i = 0; i < accuracy; i++)
    {
        for (int j = 0; j < accuracy; j++)
        {
            unsigned int current_point = (accuracy + 1) * i + j;

            vertexIndices.push_back(current_point);
            vertexIndices.push_back(current_point + accuracy + 2);
            vertexIndices.push_back(current_point + accuracy + 1);

            vertexIndices.push_back(current_point);
            vertexIndices.push_back(current_point + 1);
            vertexIndices.push_back(current_point + accuracy + 2);
        }
    }
};

void Sphere::bindToGPU()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // position coordinates
    size_t vertexBufferSize = sizeof(float) * vertexPositions.size();
    glGenBuffers(1, &posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, posVbo);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertexPositions.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    // normal coordinates
    size_t normalBufferSize = sizeof(float) * vertexNormals.size();
    glGenBuffers(1, &norVbo);
    glBindBuffer(GL_ARRAY_BUFFER, norVbo);
    glBufferData(GL_ARRAY_BUFFER, normalBufferSize, vertexNormals.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);

    // texture coordinates
    size_t textureBufferSize = sizeof(float) * vertexTextures.size();
    glGenBuffers(1, &texVbo);
    glBindBuffer(GL_ARRAY_BUFFER, texVbo);
    glBufferData(GL_ARRAY_BUFFER, textureBufferSize, vertexTextures.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(2);

    // triangle indices
    size_t indexBufferSize = sizeof(unsigned int) * vertexIndices.size();
    glGenBuffers(1, &indVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indVbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, vertexIndices.data(), GL_DYNAMIC_READ);

    // deactivate the VAO for now
    glBindVertexArray(0);
}

void Sphere::drawGeometry(std::map<std::string, bool> facesRendering)
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_INT, 0);
}
