#ifndef BLOCK_H
#define BLOCK_H

#include "GameObject.h"
#include "Cube.h"

class Block : public GameObject
{
private:
	CubePtr geometry;
	std::map<std::string, bool> faceRendering;

public:
	Block(Type _type, CubePtr cube, glm::vec3 _position, GLuint _selectTexture = 0, GLuint _texture = 0, bool _transparent = 0)
		: GameObject(_type, _position, _selectTexture, _texture, _transparent),
		geometry(cube),
		faceRendering{ {"right", !isEmpty()}, {"left",!isEmpty()}, {"top", !isEmpty()}, {"bottom", !isEmpty()}, {"front", !isEmpty()}, {"back", !isEmpty()} } {}

	void setFaceRendering(std::string name, bool beRendered) { faceRendering[name] = beRendered; }

	std::string getReversedFace(std::string name)
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

	void render(Shader shader) override
	{
		if (!isEmpty())
		{
			GameObject::render(shader);
			geometry->draw(faceRendering);
		}
	}

	void renderForPlayer(Shader shader) override
	{
		GameObject::renderForPlayer(shader);
		geometry->draw(faceRendering);
	}

	void freeBuffer() { geometry->freeBuffers(); }
};

typedef std::shared_ptr<Block> BlockPtr;

#endif // !BLOCK_H

