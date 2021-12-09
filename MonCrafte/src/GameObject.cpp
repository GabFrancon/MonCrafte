#include "GameObject.h"

GameObject::GameObject(GeometryPtr geo, glm::vec3 pos, float coeff, GLuint texID, GLuint selTexID, glm::vec3 lightSettings, bool transparency)
	: geometry(geo), position(pos), size(glm::vec3(coeff)), texture(texID), selectTexture(selTexID), lightCoeff(lightSettings), transparent(transparency) {}

void GameObject::fillObject(GLuint texture, bool transparency)
{
    setTexture(texture);
    setTransparency(transparency);
}

void GameObject::emptyObject()
{
    setTexture(0);
    setTransparency(true);
}

void GameObject::render(Shader shader)
{
    // send uniform values to shaders
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.f), position), size);
    shader.setMat4("transMat", model);
    shader.setBool("pointed", pointed);
    //glUniform3f(glGetUniformLocation(program, "lightCoeff"), lightCoeff.x, lightCoeff.y, lightCoeff.z);

    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, selectTexture);
}

void GameObject::renderForPlayer(Shader shader)
{
    // send uniform values to shaders
    glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.f), position), size);
    shader.setMat4("transMat", model);
    //glUniform3f(glGetUniformLocation(program, "lightCoeff"), lightCoeff.x, lightCoeff.y, lightCoeff.z);

    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
}