#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in float aLayer;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 transMat;

out vec3 fPos;
out vec3 fNor;
out vec2 fTex;
flat out int layer;

void main()
{
    vec4 realPos = transMat * vec4(aPos, 1.0);
    gl_Position = projMat * viewMat * realPos;

    fPos = vec3(realPos);
    fNor = aNor;
    fTex = aTex;
    layer = int(aLayer);
}