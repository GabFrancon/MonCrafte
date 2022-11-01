#include "Block.h"

std::string Block::getReversedFace(std::string name)
{
	if (name == "right") return "left";
	if (name == "left")  return "right";
	if (name == "top")   return "bottom";
	if (name == "bottom")return "top";
	if (name == "front") return "back";
	if (name == "back")  return "front";
	return "";
}

void Block::fillObject(Texture texture)
{
	for (auto& it : faceRendering)
		it.second = true;
	GameObject::fillObject(texture);
}

void Block::emptyObject()
{
	for (auto& it : faceRendering)
		it.second = false;
	GameObject::emptyObject();
}

bool Block::isHidden()
{
	for (auto& it : faceRendering)
	{
		if (it.second == true)
			return false;
	}
	return true;
}

void Block::addGeometry(
	std::vector<float>& vertices,
	std::vector<float>& normals,
	std::vector<float>& uvs,
	std::vector<float>& layers)
{
	float xoffset = position.x;
	float yoffset = position.y;
	float zoffset = position.z;

	float sideTexLocation   = (float)texture.getLocationInArray(0);
	float topTexLocation    = (float)texture.getLocationInArray(1);
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