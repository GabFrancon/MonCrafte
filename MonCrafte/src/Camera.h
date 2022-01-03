#ifndef CAMERA_H
#define CAMERA_H

#include "World.h"
#include "Text2D.h"

class Camera
{
public:
    Camera() {};
    Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, GLuint pointerTexture);

    glm::vec3 getPosition() const;
    glm::vec3 getViewDirection() const;
    glm::mat4 computeViewMatrix() const;
    void updateProjectionMatrix();

    void setAspectRatio(GLFWwindow* window);
    void updateCamPos(GLFWwindow* window, float deltaTime, World world);
    void processMouseMoovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void processMouseScroll(float yoffset);
    void updateCameraVectors();

    void bindView(Shader worldShader, Shader playerShader, Shader skyShader);
    void bindProjection(Shader worldShader, Shader playerShader, Shader skyShader);
    void render(Shader playerShader, Shader pointerShader, World world);

    void insertBlock(std::string texName, unsigned int position);
    void removeBlock(unsigned int position);
    std::string getCurrentBlock() const;
    void clearBuffers();

private:
    glm::vec3 camPos;
    glm::vec3 camFront;
    glm::vec3 camUp;
    glm::vec3 camRight;

    glm::vec3 worldFront;
    glm::vec3 worldUp;

    float scroll = 4.0f;
    float aspectRatio = 1.f; 
    float mouseSensitivity = 0.05f;
    float playerVelociy = 10.f;
    float near = 0.1f;                  // distance before which geometry is excluded from the rasterization process
    float far = 250.1f;                  // distance after which the geometry is excluded from the rasterization process
    float yaw = -90.f;                  // yaw euler angle
    float pitch = 0.f;                  // pitch euler angle

    int currentBlock = 0;
    std::vector<std::string> availableBlocks;
    glm::mat4 projMat;

    BlockPtr blockInHand;
    Text2D pointer;
};

#endif // !CAMERA_H


