
#define _USE_MATH_DEFINES

#include <vector>
#include <memory>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#ifndef BLOCK_H
#define BLOCK_H

class Block
{
private:
	std::vector<float> vertexPositions;
	std::vector<float> vertexNormals;
	std::vector<float> vertexTextures;

	GLuint vao = 0;
	GLuint posVbo = 0;
	GLuint norVbo = 0;
	GLuint texVbo = 0;

	float ambient = 0.3;

public:
	Block() {};
	void initGeometry();
	void render(const GLuint program, glm::mat4 transMat, const GLuint texture);
	void freeBuffer();

	float getAmbient() { return ambient; }
};

#endif // !BLOCK_H

