#include "GameObject.h"

GameObject::GameObject(glm::vec3 _position, Texture _texture, float _size)
	: position(_position), texture(_texture), size(glm::vec3(_size)), type(texture.getType())
{
    updateTransMat();
}

void GameObject::updateTransMat()
{
    transMat = glm::scale(glm::translate(glm::mat4(1.f), position), size);
}

void GameObject::fillObject(Texture texture)
{
    this->texture = texture;
    this->type = texture.getType();
}
