#include "Camera.h"

void Camera::focusCamOnPlayer(Player player)
{
    camFront = player.getFront();
    camUp = player.getUp();
    camPos = player.getPosition() - 0.5 * camFront + 0.1 * camUp;
}

glm::mat4 Camera::computeViewMatrix() const {return glm::lookAt(camPos + camFront / 4, camPos + camFront, camUp);}
glm::mat4 Camera::computeProjectionMatrix() const {return glm::perspective(glm::radians(55.f), aspectRatio, near, far);}
glm::vec3 Camera::getPosition() const { return camPos; }


void Camera::bindView(Shader worldShader, Shader playerShader, Shader skyShader)
{
    glm::mat4 viewMat = computeViewMatrix();

    worldShader.use();
    worldShader.setVec3("camPos", camPos);
    worldShader.setMat4("viewMat", viewMat);

    playerShader.use();
    playerShader.setVec3("camPos", camPos);
    playerShader.setMat4("viewMat", viewMat);

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

void Camera::setAspectRatio(glm::vec2 windowSize) { aspectRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y); }
