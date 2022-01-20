#include "Player.h"

Player::Player(glm::vec3 position, glm::vec3 front, glm::vec3 up) :
    playerPos(position), playerFront(front), worldUp(glm::vec3(0, 1, 0)),
    availableBlocks(std::vector<Texture>(10, Texture())),
    pointer(Text2D("X", 1, glm::vec2(0.5, 0.5), 40))
{
    updatePlayerVectors();
    pointer.bindVBOs();

    block = std::make_shared<Block>(glm::vec3(0.f), Texture());

    // generate the VAO
    glGenVertexArrays(1, &vao);
}

void Player::insertBlock(Texture tex, unsigned int position)
{
    removeBlock(position);
    availableBlocks.insert(availableBlocks.begin() + position, tex);
}

void Player::updatePosition(GLFWwindow* window, float deltaTime, World world)
{
    float cameraSpeed = playerVelociy * deltaTime;
    glm::vec3 newPos = playerPos;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        newPos += worldFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        newPos -= worldFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        newPos += playerRight * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        newPos -= playerRight * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        newPos += worldUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        newPos -= worldUp * cameraSpeed;

    if (newPos != playerPos)
        if (!world.collide(newPos))
            playerPos = newPos;
}

void Player::setPointerRatio(glm::vec2 windowSize, Shader pointerShader)
{
    pointerShader.use();
    pointerShader.setVec2("viewport", windowSize);
    pointerShader.setVec2("offset", pointer.getOffset() * windowSize - glm::vec2(std::ceil(pointer.getLength() * pointer.getSize() / 2), 0));
}

void Player::processMouseMoovement(float xoffset, float yoffset, GLboolean constrainPitch)
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
    updatePlayerVectors();
}

void Player::processMouseScroll(float yoffset)
{
    scroll -= yoffset;
    if (scroll < -0.5f)
        scroll += 9.4f;
    else if (scroll > 9.5f)
        scroll -= 9.4f;

    int newBlock = std::round(scroll);
    regenRequired = newBlock != currentBlock;
    currentBlock = newBlock;
}

void Player::updatePlayerVectors()
{
    float x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    float y = std::sin(glm::radians(pitch));
    float z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

    playerFront = glm::normalize(glm::vec3(x, y, z));
    worldFront = glm::normalize(glm::vec3(x, 0.0, z));
    playerRight = glm::normalize(glm::cross(playerFront, worldUp));
    playerUp = glm::normalize(glm::cross(playerRight, playerFront));
}

void Player::regenBlock()
{
    // first clear CPU data
    vertices.clear();
    normals.clear();
    uvs.clear();
    texIndices.clear();

    // then regen block in hand
    block->fillObject(availableBlocks[currentBlock]);
    block->addGeometry(vertices, normals, uvs, texIndices);

    // finally clear buffers before bind GPU buffers again
    clearVBOs();
    bindVBOs();
    regenRequired = false;
}

void Player::bindVBOs()
{
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

    // tex indices
    size_t layerBufferSize = sizeof(float) * texIndices.size();
    glGenBuffers(1, &indVbo);
    glBindBuffer(GL_ARRAY_BUFFER, indVbo);
    glBufferData(GL_ARRAY_BUFFER, layerBufferSize, texIndices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

void Player::render(Shader playerShader, Shader pointerShader, World world)
{
    if (regenRequired)
        regenBlock();

    playerShader.use();
    glBindVertexArray(vao);
    // render the block in the hand of the player
    if (availableBlocks[currentBlock].getType() != Type::AIR)
    {
        playerShader.setBool("useArray", true);
        glm::mat4 transMat = glm::scale(glm::translate(glm::mat4(1.f), playerPos - playerRight / 7), glm::vec3(0.1f));
        playerShader.setMat4("transMat", transMat);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
    // render the selection over the block selected by the player
    if (world.isSelection())
    {
        playerShader.setBool("useArray", false);
        glm::mat4 transMat = glm::scale(glm::translate(glm::mat4(1.f), world.getSelection()), glm::vec3(1.01f));
        playerShader.setMat4("transMat", transMat);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
    pointerShader.use();
    pointer.render();
}

void Player::deleteVAO()
{
    glDeleteVertexArrays(1, &vao);
    pointer.deleteVAO();
    pointer.clearVBOs();
}

void Player::clearVBOs()
{
    glDeleteBuffers(1, &posVbo);
    glDeleteBuffers(1, &normVbo);
    glDeleteBuffers(1, &texVbo);
    glDeleteBuffers(1, &indVbo);
}