#include "Block.h"

Block::Block(std::shared_ptr<Cube> cube, glm::vec3 pos, GLuint texID)
    : geometry(cube), position(pos), size(glm::vec3(1.0f)), texture(texID) {};

void Block::render(const GLuint program)
{
    // send uniform values to shaders
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.f), position), glm::vec3(size));
    glUniformMatrix4fv(glGetUniformLocation(program, "transMat"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform1f(glGetUniformLocation(program, "ambient"), ambient);
    glUniform1i(glGetUniformLocation(program, "material.textureData"), 0);
    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // draw 3D model
    geometry->drawGeometry();
}

void Block::freeBuffer()
{
    geometry->freeBuffer();
}
