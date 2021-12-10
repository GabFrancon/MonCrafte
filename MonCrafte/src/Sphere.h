#ifndef SPHERE_H
#define SPHERE_H

#include "Geometry.h"

class Sphere : public Geometry
{
private:
	std::vector<unsigned int> vertexIndices;
	GLuint indVbo = 0;

public:
	Sphere();
	void initBuffers() override;
	void draw();
};

typedef std::shared_ptr<Sphere> SpherePtr;

#endif // !SPHERE_H



