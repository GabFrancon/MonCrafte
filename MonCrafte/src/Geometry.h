#ifndef GEOMETRY_H
#define GEOMETRY_H

#define _USE_MATH_DEFINES

#include "Shader.h"



class Geometry
{
protected:
	std::vector<float> vertexPositions;
	std::vector<float> vertexNormals;
	std::vector<float> vertexTextures;

	GLuint vao = 0;
	GLuint posVbo = 0;
	GLuint norVbo = 0;
	GLuint texVbo = 0;

public:
	Geometry() {};
	virtual void bindToGPU() = 0;
	virtual void drawGeometry(std::map<std::string, bool> facesRendering) = 0;

	void freeBuffer()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &posVbo);
		glDeleteBuffers(1, &norVbo);
		glDeleteBuffers(1, &texVbo);
	}
};

typedef std::shared_ptr<Geometry> GeometryPtr;

#endif // !GEOMETRY_H