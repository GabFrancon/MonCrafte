#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up) :
    camPos(position), camFront(front), camUp(up), worldUp(up)
{
    updateCameraVectors();
}

void Camera::setAspectRatio(const float a)
{
    aspectRatio = a; 
}

glm::vec3 Camera::getPosition() const
{
    return camPos;
}

glm::mat4 Camera::computeViewMatrix() const
{
    return glm::lookAt(camPos, camPos + camFront, camUp);
}

glm::mat4 Camera::computeProjectionMatrix() const 
{
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::updateCamPos(GLFWwindow* window, float deltaTime)
{
    float cameraSpeed = 15.f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camPos += camFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camPos -= camFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camPos -= camRight * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camPos += camRight * cameraSpeed;
}

void Camera::processMouseMoovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;
    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    fov -= yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    front.y = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    front.z = std::sin(glm::radians(pitch));

    camFront = glm::normalize(front);
    camRight = glm::normalize(glm::cross(camFront, worldUp));
    camUp = glm::normalize(glm::cross(camRight, camFront));
}
