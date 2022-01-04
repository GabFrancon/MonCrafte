#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Shader.h"

class GameObject
{
protected:
	glm::vec3 position;
	glm::vec3 size;
	glm::mat4 transMat;
	Texture texture;
	Type type;

	bool pointed = false;

public:
	GameObject(glm::vec3 _position, Texture _texture, float _size=1.0f);

	glm::vec3 getPosition() const { return position; }
	Texture getTexture() const { return texture; }

	bool isPointed() const { return pointed; }
	bool isSolid() { return type == Type::SOLID; }
	bool isTransparent() { return type == Type::TRANSPARENT; }
	bool isEmpty() { return type == Type::AIR; }

	void setPosition(glm::vec3 position) { this->position = position;  updateTransMat(); }
	void setSize(float size) { this->size = glm::vec3(size);  updateTransMat(); }
	void setPointed(bool pointed) { this->pointed = pointed; }
	virtual void fillObject(Texture texture);
	virtual void emptyObject() { this->type = Type::AIR; }
	void updateTransMat();
};

typedef std::shared_ptr<GameObject> GameObjectPtr;

#endif // !GAMEOBJECT_H



