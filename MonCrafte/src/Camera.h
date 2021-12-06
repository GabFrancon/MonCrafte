#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "World.h"

#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
public:
    Camera() {};
    Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up);

    glm::vec3 getPosition() const;
    glm::vec3 getViewDirection() const;
    glm::mat4 computeViewMatrix() const;
    glm::mat4 computeProjectionMatrix() const;

    void setAspectRatio(GLFWwindow* window);
    void updateCamPos(GLFWwindow* window, float deltaTime, World world);
    void processMouseMoovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void processMouseScroll(float yoffset);
    void updateCameraVectors();
    void render(GLFWwindow* window, const GLuint program, const float deltaTime, World world);

private:
    glm::vec3 camPos;
    glm::vec3 camFront;
    glm::vec3 camUp;
    glm::vec3 camRight;
    glm::vec3 worldUp;

    float fov = 45.f;                   // field of view, in degrees
    float aspectRatio = 1.f;            // ratio between the width and the height of the image
    float mouseSensitivity = 0.05f;     // sensitivity coefficient of mouse movements
    float playerVelociy = 4.f;          // sensitivity coefficient of player deplacement
    float near = 0.1f;                  // distance before which geometry is excluded from the rasterization process
    float far = 250.1f;                 // distance after which the geometry is excluded from the rasterization process
    float yaw = -90.f;                  // yaw euler angle
    float pitch = 0.f;                  // pitch euler angle
};

#endif // !CAMERA_H


