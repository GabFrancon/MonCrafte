#ifndef BLOCK_H
#define BLOCK_H

#include "GameObject.h"
#include "Cube.h"

class Block : public GameObject
{
private:
	std::map<std::string, bool> faceRendering;

public:
	Block(CubePtr cube, glm::vec3 pos, GLuint texID, GLuint selTexID, bool transparency)
		: GameObject(cube, pos, 1.0f, texID, selTexID, glm::vec3(0.3, 1.0, 0.5), transparency),
		faceRendering{ {"right", true}, {"left", true}, {"top", true}, {"bottom", true}, {"front", true}, {"back", true} } {}

	Block(CubePtr cube, glm::vec3 pos, GLuint selTexID)
		: GameObject(cube, pos, 1.0f, 0, selTexID, glm::vec3(0.3, 1.0, 0.5), true),
		faceRendering{ {"right", false}, {"left", false}, {"top", false}, {"bottom", false}, {"front", false}, {"back", false} } {}

	void setFaceRendering(std::string name, bool beRendered) { faceRendering[name] = beRendered; }
	std::string getReverseFace(std::string name)
	{
		if (name == "right") return "left";
		if (name == "left")  return "right";
		if (name == "top")   return "bottom";
		if (name == "bottom")return "top";
		if (name == "front") return "back";
		if (name == "back")  return "front";
	}

	void fillObject(GLuint texture, bool transparency) override
	{
		for (auto& it : faceRendering)
			it.second = true;
		GameObject::fillObject(texture, transparency);
	}

	void emptyObject() override
	{
		for (auto& it : faceRendering)
			it.second = false;
		GameObject::emptyObject();
	}

	void render(GLuint program) override
	{
		GameObject::render(program);
		if(!isEmpty())
			geometry->drawGeometry(faceRendering);
	}
};

typedef std::shared_ptr<Block> BlockPtr;

#endif // !BLOCK_H

