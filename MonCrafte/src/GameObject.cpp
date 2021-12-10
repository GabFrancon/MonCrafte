#include "GameObject.h"

GameObject::GameObject(Type _type, glm::vec3 _position, GLuint _selectTexture, GLuint _texture, bool _transparent, float _size, glm::vec3 _lightCoeff)
	: type(_type), position(_position), selectTexture(_selectTexture), texture(_texture), transparent(_transparent), size(glm::vec3(_size)), lightCoeff(_lightCoeff)
{
    updateTransMat();
}

void GameObject::fillObject(GLuint texture, bool transparent)
{
    setTexture(texture);
    setTransparent(transparent);
    type = Type::SOLID;
}

void GameObject::emptyObject()
{
    type = Type::AIR;
}

void GameObject::render(Shader shader)
{
    if (!isEmpty())
    {
        // send uniform values to shaders
        shader.setMat4("transMat", transMat);
        shader.setBool("pointed", pointed);
        shader.setVec3("lightCoeff", lightCoeff);

        // bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, selectTexture);
    }
}

void GameObject::renderForPlayer(Shader shader)
{
    // send uniform values to shaders
    shader.setMat4("transMat", transMat);
    shader.setVec3("lightCoeff", lightCoeff);

    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
}