#include "GameObject.h"

GameObject::GameObject(Type _type, glm::vec3 _position, Texture _texture, bool _transparent, float _size, glm::vec3 _lightCoeff)
	: type(_type), position(_position), texture(_texture), transparent(_transparent), size(glm::vec3(_size)), lightCoeff(_lightCoeff)
{
    updateTransMat();
}

void GameObject::fillObject(Texture texture, bool transparent)
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
    // send uniform values to shaders
    shader.setMat4("transMat", transMat);
    shader.setBool("pointed", pointed);
    shader.setVec3("lightCoeff", lightCoeff);
}