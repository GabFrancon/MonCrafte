#ifndef CAMERA_H
#define CAMERA_H

#include "Player.h"

class Camera
{
public:
    Camera() {};

    void focusCamOnPlayer(Player player);

    glm::mat4 computeViewMatrix() const;
    glm::mat4 computeProjectionMatrix() const;
    glm::vec3 getPosition() const;

    void bindView(Shader worldShader, Shader playerShader, Shader skyShader);
    void bindProjection(Shader worldShader, Shader playerShader, Shader skyShader);
    void setAspectRatio(glm::vec2 windowSize);

private:
    glm::vec3 camPos;
    glm::vec3 camFront;
    glm::vec3 camUp;

    float aspectRatio = 1.f; 
    float near = 0.1f;                   // distance before which geometry is excluded from the rasterization process
    float far = 200.1f;                  // distance after which the geometry is excluded from the rasterization process
};

#endif // !CAMERA_H


