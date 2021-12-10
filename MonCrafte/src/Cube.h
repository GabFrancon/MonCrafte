#ifndef CUBE_H
#define CUBE_H

#include "Geometry.h"

class Cube : public Geometry
{
public:
	Cube();
	void initBuffers() override;
	void draw(std::map<std::string, bool> facesRendering);
};

typedef std::shared_ptr<Cube> CubePtr;

#endif // !CUBE_H



