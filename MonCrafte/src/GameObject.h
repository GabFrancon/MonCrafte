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
	GLuint selectTexture = 0;
	glm::vec3 lightCoeff;
	//bool rendering = true;
	bool pointed = false;
	bool transparent = false;

public:
	GameObject(GeometryPtr geo, glm::vec3 pos, float coeff, GLuint texID, GLuint selTexID, glm::vec3 lightSettings, bool transparency);
	GameObject(){};

	glm::vec3 getPosition() const { return position; }
	GLuint getTexture() const { return texture; }

	bool bePointed() const { return pointed; }
	bool isTransparent() const { return transparent; }
	bool isEmpty() { return texture == 0; }

	void setPosition(glm::vec3 position) { this->position = position; }
	void setSize(float coeff) { this->size = glm::vec3(coeff); }
	void setTexture(GLuint texture) { this->texture = texture; }
	void setPointed(bool pointed) { this->pointed = pointed; }
	void setTransparency(bool transparent) { this->transparent = transparent; }

	virtual void fillObject(GLuint texture, bool transparency);
	virtual void emptyObject();
	virtual void render(const GLuint program);

	void freeBuffer() { geometry->freeBuffer(); }
};

typedef std::shared_ptr<GameObject> GameObjectPtr;

#endif // !GAMEOBJECT_H



