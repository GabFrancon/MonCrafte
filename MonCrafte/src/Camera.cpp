#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up) :
    camPos(position), camFront(front), camUp(up), worldUp(up)
{
    updateCameraVectors();
}

void Camera::setAspectRatio(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

glm::vec3 Camera::getPosition() const
{
    return camPos;
}
glm::vec3 Camera::getViewDirection() const
{
    return camFront;
}

glm::mat4 Camera::computeViewMatrix() const
{
    return glm::lookAt(camPos, camPos + camFront, camUp);
}

glm::mat4 Camera::computeProjectionMatrix() const 
{
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::updateCamPos(GLFWwindow* window, float deltaTime, World world)
{
    float cameraSpeed = playerVelociy * deltaTime;
    glm::vec3 newPos = camPos;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        newPos += glm::vec3(camFront.x, 0.0, camFront.z) * cameraSpeed;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        newPos -= glm::vec3(camFront.x, 0.0, camFront.z) * cameraSpeed;
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        newPos += camRight * cameraSpeed;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        newPos -= camRight * cameraSpeed;
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        newPos += worldUp * cameraSpeed / 2;
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        newPos -= worldUp * cameraSpeed / 2;
    if (!world.intersect(newPos))
        camPos = newPos;
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
    front.y = std::sin(glm::radians(pitch));
    front.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

    camFront = glm::normalize(front);
    camRight = glm::normalize(glm::cross(camFront, worldUp));
    camUp = glm::normalize(glm::cross(camRight, camFront));
}

void Camera::render(GLFWwindow* window, const GLuint program, const float deltaTime, World world)
{   
    updateCamPos(window, deltaTime, world);

    glUniformMatrix4fv(glGetUniformLocation(program, "viewMat"), 1, GL_FALSE, glm::value_ptr(computeViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(program, "projMat"), 1, GL_FALSE, glm::value_ptr(computeProjectionMatrix()));
    glUniform3f(glGetUniformLocation(program, "camPos"), camPos[0], camPos[1], camPos[2]);
}
