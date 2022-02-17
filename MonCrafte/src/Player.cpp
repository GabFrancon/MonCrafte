#include "Player.h"

Player::Player(glm::vec3 position, float pitch, float yaw) :
    playerPos(position), pitch(pitch), yaw(yaw), worldUp(glm::vec3(0, 1, 0)),
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

void Player::updatePosition(GLFWwindow* window, float deltaTime, World world, bool dynamic)
{
    // dealing with horizontal displacement
    glm::vec3 newPosH = playerPos;
    float speed = horizontalSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        newPosH += worldFront  * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        newPosH -= worldFront  * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        newPosH += playerRight * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        newPosH -= playerRight * speed;

    if (!world.collide(newPosH))
        playerPos = newPosH;

    // dealing with vertical displacement
    glm::vec3 newPosV = playerPos;
    if (dynamic)
    {
        glm::vec3 force = gravity * M;

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            if (!onJump)
            {
                force += glm::vec3(0.f, 15.f, 0.f);
                onJump = true;
            }

        playerMomentum += deltaTime * force;
        playerVelocity += playerMomentum / M;

        // frictional forces induce velocity limitation
        if (playerVelocity.y < -40)
            playerVelocity.y = -40;
        if (playerVelocity.y > 20)
            playerVelocity.y = 20;

        newPosV += deltaTime * playerVelocity;

        if (world.collideOnY(newPosV) && newPosV.y < playerPos.y)
        {
            playerMomentum.y = 0;
            playerVelocity.y = 0;
            onJump = false;
        }
        else
          playerPos = newPosV;
    }
    else
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            newPosV += worldUp * speed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            newPosV -= worldUp * speed;

        if (!world.collideOnY(newPosV))
            playerPos = newPosV;
    }
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
        glm::mat4 transMat = glm::translate(glm::mat4(1.f), playerPos - playerRight / 7);
        transMat = glm::rotate(transMat, glm::radians(pitch), playerRight);
        transMat = glm::scale(transMat, glm::vec3(0.1f));
        playerShader.setMat4("transMat", transMat);
        playerShader.setBool("material.useArrayTex", true);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
    // render the selection over the block selected by the player
    if (world.isSelection())
    {
        glm::mat4 transMat = glm::translate(glm::mat4(1.f), world.getSelection());
        transMat = glm::scale(transMat, glm::vec3(1.01f));
        playerShader.setMat4("transMat", transMat);
        playerShader.setBool("material.useArrayTex", false);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
    // render the player pointer
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