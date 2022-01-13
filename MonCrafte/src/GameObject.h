#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Shader.h"

class GameObject
{
protected:
	glm::vec3 position;
	Texture texture;
	Type type;

public:
	GameObject(glm::vec3 _position, Texture _texture)
		: position(_position), texture(_texture), type(texture.getType()) {}

	glm::vec3 getPosition() const { return position; }
	Texture getTexture() const { return texture; }

	bool isSolid() { return type == Type::SOLID; }
	bool isTransparent() { return type == Type::TRANSPARENT; }
	bool isEmpty() { return type == Type::AIR; }

	void setPosition(glm::vec3 position) { this->position = position;}

	virtual void fillObject(Texture texture)
	{
		this->texture = texture;
		this->type = texture.getType();
	}

	virtual void emptyObject() { this->type = Type::AIR; }
};

typedef std::shared_ptr<GameObject> GameObjectPtr;

#endif // !GAMEOBJECT_H



