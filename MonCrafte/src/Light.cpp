#include "Light.h"

Light::Light(std::shared_ptr<Sphere> sphere, glm::vec3 pos, glm::vec3 color, GLuint texID)
    : geometry(sphere), lightPos(pos), lightColor(color), texture(texID) {};

void Light::render(const GLuint program)
{
    // send uniform values to shaders
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.f), lightPos), glm::vec3(lightSize));
    glUniformMatrix4fv(glGetUniformLocation(program, "transMat"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);
    glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor[0], lightColor[1], lightColor[2]);
    glUniform1f(glGetUniformLocation(program, "ambient"), ambient);
    glUniform1i(glGetUniformLocation(program, "material.textureData"), 0);
    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // draw 3D model
    geometry->drawGeometry();
}

void Light::freeBuffer()
{
    geometry->freeBuffer();
}
