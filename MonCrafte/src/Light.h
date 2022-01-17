#ifndef LIGHT_H
#define LIGHT_H

#include "Shader.h"
#include "FboShadowMap.h"

class Light
{
private:
	glm::vec3 position;
	glm::vec3 color;
	bool isActive = true;

	FboShadowMap shadowMap;
	glm::mat4 depthMVP;

public:
	Light(glm::vec3 pos, glm::vec3 lightColor, FboShadowMap fbo)
		: position(pos), color(lightColor), shadowMap(fbo) {}

	Light() {}

	void setPosition(glm::vec3 pos) { position = pos; }
	bool isOn() const { return isActive; }
	void setActivate(bool activate) { isActive = activate; }
	unsigned int getShadowMapTex() const {return shadowMap._depthMapTexture;}
	glm::mat4 getDepthMVP() const {return depthMVP;}

	void bindLight(Shader worldShader, Shader playerShader)
	{
		worldShader.use();
		worldShader.setVec3("lightPos", position);
		worldShader.setVec3("lightColor", color);

		playerShader.use();
		playerShader.setVec3("lightPos", position);
		playerShader.setVec3("lightColor", color);
	}

	void setupCameraForShadowMapping(Shader shader, const glm::vec3 scene_center, const float scene_radius, glm::vec3 camPos)
	{
		float offset = glm::length(glm::vec3(position.x - camPos.x, 0, position.z - camPos.z));
		glm::mat4 lightProjection = glm::ortho(
			- scene_radius, 
			scene_radius, 
			- scene_radius, 
			scene_radius, 
			(float)-200.1+offset, 
			(float) 200.1+offset);

		glm::mat4 lightView = glm::lookAt(position, scene_center, glm::vec3(0.0f, 1.0f, 0.0f));

		depthMVP = lightProjection * lightView;
		shader.setMat4("depthMVP", depthMVP);
	}

	void bindShadowMap()
	{
		shadowMap.bindFbo();
	}
	void savePpmFile()
	{
		shadowMap.savePpmFile(std::string("shadom_map.ppm"));
	}

};

typedef std::shared_ptr<Light> LightPtr;

#endif // !LIGHT_H
