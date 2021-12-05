#define _USE_MATH_DEFINES

#include <vector>
#include <memory>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
class Sphere
{
private:
	std::vector<float> vertexPositions;
	std::vector<float> vertexNormals;
	std::vector<float> vertexTextures;
	std::vector<unsigned int> vertexIndices;

	GLuint vao = 0;
	GLuint posVbo = 0;
	GLuint norVbo = 0;
	GLuint texVbo = 0;
	GLuint indVbo = 0;

	float ambient = 2;

public:
	Sphere() {};
	void initGeometry();
	void render(const GLuint program, glm::mat4 transMat, const GLuint texture);
	void freeBuffer();

	float getAmbient() { return ambient; }
};

