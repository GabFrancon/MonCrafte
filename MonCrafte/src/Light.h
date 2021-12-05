#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Light
{
private:
	glm::vec3 lightPos;
	glm::vec3 lightColor;

public:
	Light() {};
	Light(glm::vec3 position, glm::vec3 color);

	glm::vec3 getPos();
	void setPos(glm::vec3 position);
	void render(const GLuint program);
};

