#define _MY_OPENGL_IS_33_
#define _USE_MATH_DEFINES


#include <vector>
#include <memory>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Block
{
private:
	std::vector<float> vertexPositions;
	std::vector<unsigned int> vertexIndices;

	GLuint vao = 0;
	GLuint posVbo = 0;
	GLuint indVbo = 0;

public:
	Block();
	void initGeometry();
	void bindToGPU();
	void draw();
	void freeBuffer();
};

