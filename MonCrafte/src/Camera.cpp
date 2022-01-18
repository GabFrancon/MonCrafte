#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, GLuint fontTexture, GLuint selectionTexture) :
    camPos(position), camFront(front), camUp(up), worldUp(up), 
    availableBlocks(std::vector<std::string>(10, "None")), 
    pointer(Text2D("X", 1, glm::vec2(0.5, 0.5), 40)),
    fontTex(fontTexture), selectionTex(selectionTexture)
{
    updateCameraVectors();
    pointer.bindVBOs();

    Texture tex;
    tex.addSample(0, 0);
    tex.setType(Type::SOLID);

    block = std::make_shared<Block>(glm::vec3(0.f), tex);

    std::vector<float> layers;
    block->addGeometry(vertices, normals, uvs, layers);

    // generate the VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // position coordinates
    glGenBuffers(1, &posVbo);
    size_t vertexBufferSize = sizeof(float) * vertices.size();
    glBindBuffer(GL_ARRAY_BUFFER, posVbo);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal coordinates
    glGenBuffers(1, &normVbo);
    size_t normalBufferSize = sizeof(float) * normals.size();
    glBindBuffer(GL_ARRAY_BUFFER, normVbo);
    glBufferData(GL_ARRAY_BUFFER, normalBufferSize, normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // texture coordinates
    glGenBuffers(1, &texVbo);
    size_t textureBufferSize = sizeof(float) * uvs.size();
    glBindBuffer(GL_ARRAY_BUFFER, texVbo);
    glBufferData(GL_ARRAY_BUFFER, textureBufferSize, uvs.data(), GL_STATIC_DRAW); 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Camera::clearBuffers()
{
    pointer.deleteVAO();
    pointer.clearVBOs();

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &posVbo);
    glDeleteBuffers(1, &normVbo);
    glDeleteBuffers(1, &texVbo);
}

void Camera::setAspectRatio(glm::vec2 windowSize, Shader pointerShader)
{
    aspectRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
    
    pointerShader.use();
    pointerShader.setVec2("viewport", windowSize);
    pointerShader.setVec2("offset", pointer.getOffset() * windowSize - glm::vec2(std::ceil(pointer.getLength() * pointer.getSize() / 2), 0));
}

glm::vec3 Camera::getPosition() const {return camPos;}

glm::vec3 Camera::getViewDirection() const {return camFront;}

glm::mat4 Camera::computeViewMatrix() const {return glm::lookAt(camPos, camPos + camFront, camUp);}

glm::mat4 Camera::computeProjectionMatrix() const {return glm::perspective(glm::radians(55.f), aspectRatio, near, far);}

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        newPos += worldFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        newPos -= worldFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        newPos += camRight * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        newPos -= camRight * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        newPos += worldUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        newPos -= worldUp * cameraSpeed;

    if (newPos != camPos)
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
    float x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    float y = std::sin(glm::radians(pitch));
    float z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

    camFront   = glm::normalize(glm::vec3(x, y, z));
    worldFront = glm::normalize(glm::vec3(x, 0.0, z));
    camRight   = glm::normalize(glm::cross(camFront, worldUp));
    camUp      = glm::normalize(glm::cross(camRight, camFront));
}

void Camera::bindView(Shader worldShader, Shader skyShader)
{
    glm::mat4 viewMat = computeViewMatrix();

    worldShader.use();
    worldShader.setVec3("camPos", camPos);
    worldShader.setMat4("viewMat", viewMat);

    skyShader.use();
    skyShader.setMat4("viewMat", glm::mat4(glm::mat3(viewMat)));
}

void Camera::bindProjection(Shader worldShader, Shader playerShader, Shader skyShader)
{
    glm::mat4 projMat = computeProjectionMatrix();

    worldShader.use();
    worldShader.setMat4("projMat", projMat);

    playerShader.use();
    playerShader.setMat4("projMat", projMat);

    skyShader.use();
    skyShader.setMat4("projMat", projMat);
}

void Camera::render(Shader playerShader, Shader pointerShader, World world, glm::vec2 windowSize)
{   
    glActiveTexture(GL_TEXTURE0);
    glm::mat4 viewMat = computeViewMatrix();

    playerShader.use();
    playerShader.setVec3("camPos", camPos);
    glBindVertexArray(vao);
  
    // render the block in the hand of the player
    if (availableBlocks[currentBlock] != "None")
    {
        playerShader.setMat4("viewMat", glm::mat4(glm::mat3(viewMat)));
        playerShader.setMat4("transMat", glm::scale(glm::translate(glm::mat4(1.f), camFront / 4 - camRight / 7 - camUp / 9), glm::vec3(0.1f)));
        glBindTexture(GL_TEXTURE_2D, world.getTexture(availableBlocks[currentBlock]).getTexID(0));
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }

    // render the selection over the block selected by the player
    if (world.isSelection())
    {
        playerShader.setMat4("viewMat", viewMat);
        playerShader.setMat4("transMat", glm::scale(glm::translate(glm::mat4(1.f), world.getSelection()), glm::vec3(1.01f)));
        glBindTexture(GL_TEXTURE_2D, selectionTex);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }

    glBindTexture(GL_TEXTURE_2D, fontTex);
    pointerShader.use();
    pointer.render();
}
