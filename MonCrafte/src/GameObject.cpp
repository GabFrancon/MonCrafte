#include "GameObject.h"

GameObject::GameObject(GeometryPtr geo, glm::vec3 pos, float coeff, GLuint texID, GLuint selTexID, glm::vec3 lightSettings, bool transparency)
	: geometry(geo), position(pos), size(glm::vec3(coeff)), texture(texID), selectTexture(selTexID), lightCoeff(lightSettings), transparent(transparency) {}

void GameObject::render(const GLuint program)
{
    // send uniform values to shaders
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.f), position), size);
    glUniformMatrix4fv(glGetUniformLocation(program, "transMat"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(program, "lightCoeff"), lightCoeff.x, lightCoeff.y, lightCoeff.z);
    glUniform1i(glGetUniformLocation(program, "material.textureData"), 0);
    glUniform1i(glGetUniformLocation(program, "material.textureSelect"), 1);
    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (pointed)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, selectTexture);
    }
    else
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    // draw 3D model
    geometry->drawGeometry();
}