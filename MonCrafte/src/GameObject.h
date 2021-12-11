#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Shader.h"
#include "Texture.h"

enum class Type {AIR, SOLID};

class GameObject
{
protected:
	Type type;
	glm::vec3 lightCoeff;
	glm::vec3 position;
	glm::vec3 size;
	glm::mat4 transMat;

	Texture texture;

	bool pointed = false;
	bool transparent = false;

public:
	GameObject(Type _type, glm::vec3 _position, Texture _texture, bool _transparent, float _size=1.0f, glm::vec3 _lightCoeff=glm::vec3(0.3, 1.0, 0.5));

	glm::vec3 getPosition() const { return position; }
	Texture getTexture() const { return texture; }

	bool isPointed() const { return pointed; }
	bool isTransparent() const { return transparent; }
	bool isEmpty() { return type == Type::AIR; }

	void setPosition(glm::vec3 position) { this->position = position;  updateTransMat(); }
	void setSize(float size) { this->size = glm::vec3(size);  updateTransMat(); }
	void setTexture(Texture texture) { this->texture = texture; }
	void setPointed(bool pointed) { this->pointed = pointed; }
	void setTransparent(bool transparent) { this->transparent = transparent; }
	void updateTransMat() { transMat = glm::scale(glm::translate(glm::mat4(1.f), position), size); }

	virtual void fillObject(Texture texture, bool transparent);
	virtual void emptyObject();
	virtual void render(Shader shader);
};

typedef std::shared_ptr<GameObject> GameObjectPtr;

#endif // !GAMEOBJECT_H



