#include "Block.h"

Block::Block(){}

void Block::initGeometry()
{
    vertexPositions = {
        // front
        -1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        // back
        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0
    };
    vertexIndices = {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };
}

void Block::bindToGPU()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate a GPU buffer to store the positions of the vertices
    size_t vertexBufferSize = sizeof(float) * vertexPositions.size(); 
    glGenBuffers(1, &posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, posVbo);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertexPositions.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    // Same for the indices of the triangles forming the mesh
    size_t indexBufferSize = sizeof(unsigned int) * vertexIndices.size();
    glGenBuffers(1, &indVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indVbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, vertexIndices.data(), GL_DYNAMIC_READ);

    // deactivate the VAO for now
     glBindVertexArray(0);
}

void Block::draw()
{
    // activate the VAO storing geometry data
    glBindVertexArray(vao);
    // Call for rendering: stream the current GPU geometry through the current GPU program
    glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_INT,0);
}

void Block::freeBuffer()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &posVbo);
    glDeleteBuffers(1, &indVbo);
}
