#include "Camera.h"

Camera::Camera(World world, glm::vec3 position, glm::vec3 front, glm::vec3 up, GLuint pointerTexture) :
    camPos(position), camFront(front), camUp(up), worldUp(up), availableBlocks(std::vector<std::string>(10, "None")), pointer(Text2D("@", 1, 388, 288, 35, pointerTexture))
{
    updateCameraVectors();

    blockInHand = std::make_shared<Block>(
        Type::SOLID,
        world.getCubeGeometry(), 
        glm::vec3(0.f),
        Texture());

    blockInHand->setSize(0.1);
    pointer.initBuffers();
}

void Camera::clearBuffers() { pointer.freeBuffers(); }

void Camera::setAspectRatio(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    updateProjectionMatrix();
}

glm::vec3 Camera::getPosition() const {return camPos;}

glm::vec3 Camera::getViewDirection() const {return camFront;}

glm::mat4 Camera::computeViewMatrix() const {return glm::lookAt(camPos, camPos + camFront, camUp);}

void Camera::updateProjectionMatrix() {projMat = glm::perspective(glm::radians(55.f), aspectRatio, near, far);}

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
        scroll += 9.4f;
    else if (scroll > 9.5f)
        scroll -= 9.4f;
    
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

void Camera::bindView(Shader worldShader, Shader playerShader, Shader skyShader)
{
    glm::mat4 viewMat = computeViewMatrix();

    worldShader.use();
    worldShader.setMat4("viewMat", viewMat);

    playerShader.use();
    playerShader.setMat4("viewMat", viewMat);

    skyShader.use();
    skyShader.setMat4("viewMat", glm::mat4(glm::mat3(viewMat)));
}

void Camera::bindProjection(Shader worldShader, Shader playerShader, Shader skyShader)
{
    worldShader.use();
    worldShader.setMat4("projMat", projMat);

    playerShader.use();
    playerShader.setMat4("projMat", projMat);

    skyShader.use();
    skyShader.setMat4("projMat", projMat);
}

void Camera::render(Shader playerShader, Shader pointerShader, World world)
{   
    playerShader.use();
    try {
        if (availableBlocks[currentBlock] != "None")
        {
            blockInHand->setPosition(camPos + camFront / 4 - camRight / 6 - camUp / 7);
            blockInHand->setTexture(world.getTexture(availableBlocks[currentBlock]));
            blockInHand->render(playerShader);
        }
    }
    catch (...) { std::cout << "failed to render player" << std::endl; }

    pointer.render(pointerShader);
}
