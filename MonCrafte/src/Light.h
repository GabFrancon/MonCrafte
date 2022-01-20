#ifndef LIGHT_H
#define LIGHT_H

#include "utils.h"
#include "Shader.h"

struct FboShadowMap
{
public:
	GLuint getTextureId() const { return _depthMapTexture; }

	void bindFbo()
	{
		glViewport(0, 0, _depthMapTextureWidth, _depthMapTextureHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFbo);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void free() { glDeleteFramebuffers(1, &_depthMapFbo); }

	void savePpmFile(std::string const& filename)
	{
		std::ofstream output_image(filename.c_str());

		// READ THE PIXELS VALUES from FBO AND SAVE TO A .PPM FILE
		int i, j, k;
		float* pixels = new float[_depthMapTextureWidth * _depthMapTextureHeight];

		// READ THE CONTENT FROM THE FBO
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(0, 0, _depthMapTextureWidth, _depthMapTextureHeight, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);

		output_image << "P3" << std::endl;
		output_image << _depthMapTextureWidth << " " << _depthMapTextureHeight << std::endl;
		output_image << "255" << std::endl;

		k = 0;
		for (i = 0; i < _depthMapTextureWidth; ++i) {
			for (j = 0; j < _depthMapTextureHeight; ++j) {
				output_image <<
					static_cast<unsigned int>(255 * pixels[k]) << " " <<
					static_cast<unsigned int>(255 * pixels[k]) << " " <<
					static_cast<unsigned int>(255 * pixels[k]) << " ";
				k = k + 1;
			}
			output_image << std::endl;
		}
		delete[] pixels;
		output_image.close();
	}

	GLuint _depthMapFbo = 0;
	GLuint _depthMapTexture = 0;
	unsigned int _depthMapTextureWidth = 0;
	unsigned int _depthMapTextureHeight = 0;
};


class Light
{
private:
	glm::vec3 position;
	glm::vec3 color;

	FboShadowMap shadowMap;
	glm::mat4 depthMVP;
	GLuint shadowMapOnGPU = 0;

public:
	Light() {}
	Light(glm::vec3 pos, glm::vec3 lightColor) : position(pos), color(lightColor) {}

	void setPosition(glm::vec3 pos) { position = pos; }
	void setShadowMapOnGPU(GLuint index) { shadowMapOnGPU = index; }
	GLuint getShadowMapOnGPU() { return shadowMapOnGPU; }
	void setFBO(FboShadowMap fbo) { shadowMap = fbo; }
	unsigned int getShadowMapTex() const {return shadowMap._depthMapTexture;}
	glm::mat4 getDepthMVP() const {return depthMVP;}

	void bindLight(Shader worldShader, Shader playerShader)
	{
		worldShader.use();
		worldShader.setVec3("lightPos", position);
		worldShader.setVec3("lightColor", color);
		worldShader.setMat4("depthMVP", depthMVP);
		worldShader.setInt("depthMap", shadowMapOnGPU);

		playerShader.use();
		playerShader.setVec3("lightPos", position);
		playerShader.setVec3("lightColor", color);
	}

	void setupCameraForShadowMapping(const glm::vec3 scene_center, const float scene_radius, glm::vec3 camPos)
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
	}

	void bindShadowMap() { shadowMap.bindFbo(); }
	void savePpmFile() { shadowMap.savePpmFile(std::string("shadom_map.ppm")); }
};

#endif // !LIGHT_H
