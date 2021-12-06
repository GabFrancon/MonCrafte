#include "Geometry.h"

#ifndef CUBE_H
#define CUBE_H

class Cube : public Geometry
{
public:
	Cube();
	void bindToGPU() override;
	void drawGeometry() override;
};

#endif // !CUBE_H



