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

	void fillObject(Texture texture) override
	{
		for (auto& it : faceRendering)
			it.second = true;
		GameObject::fillObject(texture);
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

	void addGeometry(
		std::vector<float> &vertices, 
		std::vector<float> &normals, 
		std::vector<float> &uvs, 
		std::vector<float> &layers)
	{
		float xoffset = position.x;
		float yoffset = position.y;
		float zoffset = position.z;

		float sideTexLocation = (float)texture.getLocationInArray(0);
		float topTexLocation = (float)texture.getLocationInArray(1);
		float bottomTexLocation = (float)texture.getLocationInArray(2);

		if (isFaceVisible("right"))
		{
			vertices.insert(vertices.end(), {
					0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
					0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
					0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
					0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
					0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
					0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset
				});
			normals.insert(normals.end(), {
					1.0f, 0.0f, 0.0f,
					1.0f, 0.0f, 0.0f,
					1.0f, 0.0f, 0.0f,
					1.0f, 0.0f, 0.0f,
					1.0f, 0.0f, 0.0f,
					1.0f, 0.0f, 0.0f
				});
			uvs.insert(uvs.end(), {
					1.0f, 0.0f,
					0.0f, 1.0f,
					0.0f, 0.0f,
					1.0f, 0.0f,
					1.0f, 1.0f,
					0.0f, 1.0f
				});
			layers.insert(layers.end(), {
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation
				});
		}
		if (isFaceVisible("left"))
		{
			vertices.insert(vertices.end(), {
					-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
					-0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
					-0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
					-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
					-0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
					-0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset
				});
			normals.insert(normals.end(), {
					-1.0f, 0.0f, 0.0f,
					-1.0f, 0.0f, 0.0f,
					-1.0f, 0.0f, 0.0f,
					-1.0f, 0.0f, 0.0f,
					-1.0f, 0.0f, 0.0f,
					-1.0f, 0.0f, 0.0f
				});
			uvs.insert(uvs.end(), {
					0.0f, 1.0f,
					1.0f, 1.0f,
					1.0f, 0.0f,
					0.0f, 1.0f,
					1.0f, 0.0f,
					0.0f, 0.0f
				});
			layers.insert(layers.end(), {
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation
				});
		}
		if (isFaceVisible("top"))
		{
			vertices.insert(vertices.end(), {
					 0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
					 0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
					-0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
					 0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
					-0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
					-0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset
				});
			normals.insert(normals.end(), {
					0.0f, 1.0f, 0.0f,
					0.0f, 1.0f, 0.0f,
					0.0f, 1.0f, 0.0f,
					0.0f, 1.0f, 0.0f,
					0.0f, 1.0f, 0.0f,
					0.0f, 1.0f, 0.0f
				});
			uvs.insert(uvs.end(), {
					1.0f, 1.0f,
					1.0f, 0.0f,
					0.0f, 0.0f,
					1.0f, 1.0f,
					0.0f, 0.0f,
					0.0f, 1.0f
				});
			layers.insert(layers.end(), {
					topTexLocation,
					topTexLocation,
					topTexLocation,
					topTexLocation,
					topTexLocation,
					topTexLocation
				});
		}
		if (isFaceVisible("bottom"))
		{
			vertices.insert(vertices.end(), {
					 0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
					-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
					 0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
					 0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
					-0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
					-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset
				});
			normals.insert(normals.end(), {
					0.0f,-1.0f, 0.0f,
					0.0f,-1.0f, 0.0f,
					0.0f,-1.0f, 0.0f,
					0.0f,-1.0f, 0.0f,
					0.0f,-1.0f, 0.0f,
					0.0f,-1.0f, 0.0f
				});
			uvs.insert(uvs.end(), {
					1.0f, 1.0f,
					0.0f, 0.0f,
					1.0f, 0.0f,
					1.0f, 1.0f,
					0.0f, 1.0f,
					0.0f, 0.0f
				});
			layers.insert(layers.end(), {
					bottomTexLocation,
					bottomTexLocation,
					bottomTexLocation,
					bottomTexLocation,
					bottomTexLocation,
					bottomTexLocation
				});
		}
		if (isFaceVisible("front"))
		{
			vertices.insert(vertices.end(), {
					-0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
					-0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
					 0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
					-0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
					 0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
					 0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset
				});
			normals.insert(normals.end(), {
					0.0f, 0.0f, 1.0f,
					0.0f, 0.0f, 1.0f,
					0.0f, 0.0f, 1.0f,
					0.0f, 0.0f, 1.0f,
					0.0f, 0.0f, 1.0f,
					0.0f, 0.0f, 1.0f
				});
			uvs.insert(uvs.end(), {
					0.0f, 0.0f,
					0.0f, 1.0f,
					1.0f, 1.0f,
					0.0f, 0.0f,
					1.0f, 1.0f,
					1.0f, 0.0f
				});
			layers.insert(layers.end(), {
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation
				});
		}
		if (isFaceVisible("back"))
		{
			vertices.insert(vertices.end(), {
					 0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
					-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
					-0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
					 0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
					 0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
					-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset
				});
			normals.insert(normals.end(), {
					0.0f, 0.0f,-1.0f,
					0.0f, 0.0f,-1.0f,
					0.0f, 0.0f,-1.0f,
					0.0f, 0.0f,-1.0f,
					0.0f, 0.0f,-1.0f,
					0.0f, 0.0f,-1.0f
				});
			uvs.insert(uvs.end(), {
					1.0f, 0.0f,
					0.0f, 1.0f,
					0.0f, 0.0f,
					1.0f, 0.0f,
					1.0f, 1.0f,
					0.0f, 1.0f
				});
			layers.insert(layers.end(), {
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation,
					sideTexLocation
				});
		}
	}
};

typedef std::shared_ptr<Block> BlockPtr;

#endif // !BLOCK_H

