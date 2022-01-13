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
    glm::mat4 computeProjectionMatrix() const;

    void setAspectRatio(glm::vec2 windowSize, Shader pointerShader);
    void updateCamPos(GLFWwindow* window, float deltaTime, World world);
    void processMouseMoovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void processMouseScroll(float yoffset);
    void updateCameraVectors();

    void bindView(Shader worldShader, Shader skyShader);
    void bindProjection(Shader worldShader, Shader playerShader, Shader skyShader);
    void render(Shader playerShader, Shader pointerShader, World world, glm::vec2 windowSize);
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
    float near = 0.1f;                   // distance before which geometry is excluded from the rasterization process
    float far = 200.1f;                  // distance after which the geometry is excluded from the rasterization process
    float yaw = -90.f;                   // yaw euler angle
    float pitch = 0.f;                   // pitch euler angle

    int currentBlock = 0;
    std::vector<std::string> availableBlocks;

    Text2D pointer;
    BlockPtr block;

    GLuint vao = 0;
    GLuint posVbo = 0;
    GLuint normVbo = 0;
    GLuint texVbo = 0;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uvs;
};

#endif // !CAMERA_H


