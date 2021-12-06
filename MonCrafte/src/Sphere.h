#define _USE_MATH_DEFINES

#include <vector>
#include "Geometry.h"

#ifndef SPHERE_H
#define SPHERE_H

class Sphere : public Geometry
{
private:
	std::vector<unsigned int> vertexIndices;
	GLuint indVbo = 0;

public:
	Sphere();
	void bindToGPU() override;
	void drawGeometry() override;
};

#endif // !SPHERE_H



