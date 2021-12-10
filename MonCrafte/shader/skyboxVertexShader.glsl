#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 aTex;

uniform mat4 projMat;
uniform mat4 viewMat;

void main()
{
    aTex = aPos;
    gl_Position = projMat * viewMat * vec4(aPos, 1.0);
}  