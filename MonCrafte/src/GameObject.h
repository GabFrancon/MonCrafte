#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Shader.h"

class GameObject
{
protected:
	glm::vec3 position;
	glm::vec3 size;
	Texture texture;
	Type type;

public:
	GameObject(glm::vec3 _position, Texture _texture, float _size=1.0f)
		: position(_position), texture(_texture), size(glm::vec3(_size)), type(texture.getType()) {}

	glm::vec3 getPosition() const { return position; }
	Texture getTexture() const { return texture; }

	bool isSolid() { return type == Type::SOLID; }
	bool isTransparent() { return type == Type::TRANSPARENT; }
	bool isEmpty() { return type == Type::AIR; }

	void setPosition(glm::vec3 position) { this->position = position;}
	void setSize(float size) { this->size = glm::vec3(size); }

	virtual void fillObject(Texture texture)
	{
		this->texture = texture;
		this->type = texture.getType();
	}

	virtual void emptyObject() { this->type = Type::AIR; }
};

typedef std::shared_ptr<GameObject> GameObjectPtr;

#endif // !GAMEOBJECT_H



