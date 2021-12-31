#ifndef BLOCK_H
#define BLOCK_H

#include "GameObject.h"

class Block : public GameObject
{
private:
	CubePtr geometry;
	std::map<std::string, bool> faceRendering;

public:
	Block(Type _type, CubePtr cube, glm::vec3 _position, Texture _texture, bool _transparent = 0)
		: GameObject(_type, _position, _texture, _transparent),
		geometry(cube),
		faceRendering{ {"right", !isEmpty()}, {"left",!isEmpty()}, {"top", !isEmpty()}, {"bottom", !isEmpty()}, {"front", !isEmpty()}, {"back", !isEmpty()} } {}

	bool isFaceVisible(std::string name) { return faceRendering[name]; }
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

	void fillObject(Texture texture, bool transparency) override
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

	bool isHidden()
	{
		for (auto& it : faceRendering)
		{
			if(it.second == true)
				return false;
		}
		return true;
	}

	void render(Shader shader) override
	{
		GameObject::render(shader);
		geometry->draw(texture, faceRendering);
	}

	void freeBuffer() { geometry->freeBuffers(); }
};

typedef std::shared_ptr<Block> BlockPtr;

#endif // !BLOCK_H

