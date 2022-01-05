#ifndef BLOCK_H
#define BLOCK_H

#include "GameObject.h"

class Block : public GameObject
{
private:
	std::map<std::string, bool> faceRendering;

public:
	Block(glm::vec3 _position, Texture _texture)
		: GameObject(_position, _texture),
		faceRendering{ {"right", !isEmpty()}, {"left",!isEmpty()}, {"top", !isEmpty()}, {"bottom", !isEmpty()}, {"front", !isEmpty()}, {"back", !isEmpty()} } {}

	bool isHidden();
	bool isFaceVisible(std::string name) { return faceRendering[name]; }
	std::string getReversedFace(std::string name);
	void fillObject(Texture texture) override;
	void emptyObject() override;
	void setFaceRendering(std::string name, bool beRendered) { faceRendering[name] = beRendered; }

	void addGeometry(
		std::vector<float>& vertices,
		std::vector<float>& normals,
		std::vector<float>& uvs,
		std::vector<float>& layers);
};

typedef std::shared_ptr<Block> BlockPtr;

#endif // !BLOCK_H

