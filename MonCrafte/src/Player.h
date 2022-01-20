#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"
#include "World.h"
#include "Text2D.h"

class Player
{
public:
    Player() {};
    Player(glm::vec3 position, glm::vec3 front, glm::vec3 up);

    glm::vec3 getPosition() const { return playerPos; }
    glm::vec3 getFront() const { return playerFront; }
    glm::vec3 getUp() const { return playerUp; }
    glm::vec3 getRight() const { return playerRight; }
    Texture getCurrentBlock() const { return availableBlocks[currentBlock]; }
    void removeBlock(unsigned int position) { availableBlocks.erase(availableBlocks.begin() + position); }

    void updatePosition(GLFWwindow* window, float deltaTime, World world);
    void setPointerRatio(glm::vec2 windowSize, Shader pointerShader);
    void processMouseMoovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void processMouseScroll(float yoffset);
    void updatePlayerVectors();

    void regenBlock();
    void bindVBOs();
    void render(Shader playerShader, Shader pointerShader, World world);
    void insertBlock(Texture tex, unsigned int position);
    void deleteVAO();
    void clearVBOs();

private:
    glm::vec3 playerPos;
    glm::vec3 playerFront;
    glm::vec3 playerUp;
    glm::vec3 playerRight;

    glm::vec3 worldFront;
    glm::vec3 worldUp;

    float scroll = 4.0f;
    float mouseSensitivity = 0.05f;
    float playerVelociy = 10.f;
    float yaw = -90.f;                   // yaw euler angle
    float pitch = 0.f;                   // pitch euler angle

    int currentBlock = 0;
    std::vector<Texture> availableBlocks;
    bool regenRequired = true;

    Text2D pointer;
    BlockPtr block;

    GLuint vao = 0;
    GLuint posVbo = 0;
    GLuint normVbo = 0;
    GLuint texVbo = 0;
    GLuint indVbo = 0;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<float> texIndices;
};

#endif // !PLAYER_H
