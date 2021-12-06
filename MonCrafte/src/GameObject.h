#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Geometry.h"

class GameObject
{
protected:
	GeometryPtr geometry;
	glm::vec3 position;
	glm::vec3 size;
	GLuint texture = 0;
	glm::vec3 lightCoeff;

public:
	GameObject(GeometryPtr geo, glm::vec3 pos, float coeff, GLuint texID, glm::vec3 lightSettings);

	glm::vec3 getPosition() const { return position; }

	void setPosition(glm::vec3 position) { this->position = position; }
	void setSize(float coeff) { this->size = glm::vec3(coeff); }
	void setTexture(GLuint texture) { this->texture = texture; }

	virtual void render(const GLuint program);
	void freeBuffer() { geometry->freeBuffer(); };
};

typedef std::shared_ptr<GameObject> GameObjectPtr;

#endif // !GAMEOBJECT_H



