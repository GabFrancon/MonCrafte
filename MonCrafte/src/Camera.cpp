#include "Camera.h"

Camera::Camera(World world, GLFWwindow* window, glm::vec3 position, glm::vec3 front, glm::vec3 up) :
    camPos(position), camFront(front), camUp(up), worldUp(up), availableBlocks(std::vector<std::string>(10, "None"))
{
    setAspectRatio(window);
    updateCameraVectors();

    blockInHand = std::make_shared<Block>(
        world.getCubeGeometry(), 
        camPos + camFront / 4 - camRight / 9 - camUp / 12, 
        world.getTexture("selection+"));

    blockInHand->setSize(0.1);

    cursor = std::make_shared<Light>(
        world.getSphereGeometry(), 
        camPos + camFront / 4, 
        0.002, 
        world.getTexture("light"), 
        glm::vec3(1.0f));
}

void Camera::setAspectRatio(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

glm::vec3 Camera::getPosition() const {return camPos;}

glm::vec3 Camera::getViewDirection() const {return camFront;}

glm::mat4 Camera::computeViewMatrix() const {return glm::lookAt(camPos, camPos + camFront, camUp);}

glm::mat4 Camera::computeProjectionMatrix() const {return glm::perspective(glm::radians(45.f), aspectRatio, near, far);}

std::string Camera::getCurrentBlock() const { return availableBlocks[currentBlock]; }

void Camera::removeBlock(unsigned int position) { availableBlocks.erase(availableBlocks.begin() + position); }

void Camera::insertBlock(std::string texName, unsigned int position)
{
    removeBlock(position);
    availableBlocks.insert(availableBlocks.begin() + position, texName); 
}

void Camera::updateCamPos(GLFWwindow* window, float deltaTime, World world)
{
    float cameraSpeed = playerVelociy * deltaTime;
    glm::vec3 newPos = camPos;
    glm::vec3 frontDir = glm::normalize(glm::vec3(camFront.x, 0.0, camFront.z));
    glm::vec3 rightDir = camRight;
    glm::vec3 upDir    = worldUp;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        newPos += frontDir * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        newPos -= frontDir * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        newPos += rightDir * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        newPos -= rightDir * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        newPos += upDir * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        newPos -= upDir * cameraSpeed;

    if (!world.collide(newPos))
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
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    scroll -= yoffset;
    if (scroll < -0.5f)
        scroll += 9.5f;
    else if (scroll > 9.5f)
        scroll -= 9.5f;
    
    currentBlock = std::round(scroll);
}

void Camera::updateCameraVectors()
{
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

    if (availableBlocks[currentBlock] != "None")
    {
        blockInHand->setPosition(camPos + camFront / 4 - camRight / 9 - camUp / 12);
        blockInHand->setTexture(world.getTexture(availableBlocks[currentBlock]));
        blockInHand->render(program);
    }

    cursor->setPosition(camPos + camFront / 4);
    cursor->render(program);
}
