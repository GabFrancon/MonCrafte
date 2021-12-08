#ifndef CUBE_H
#define CUBE_H

#include "Geometry.h"

class Cube : public Geometry
{
public:
	Cube();
	void bindToGPU() override;
	void drawGeometry(std::map<std::string, bool> facesRendering) override;
};

typedef std::shared_ptr<Cube> CubePtr;

#endif // !CUBE_H



