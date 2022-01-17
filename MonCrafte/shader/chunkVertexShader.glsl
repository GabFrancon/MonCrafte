#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in float aLayer;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 depthMVP;

uniform vec3 lightPos;

out vec3 fPos;
out vec3 fPosModel;
out vec3 fNor;
out vec2 fTex;
flat out int layer;

void main()
{
    gl_Position = projMat * viewMat * vec4(aPos, 1.0);
    fPos = aPos;
    vec3 norm = 1.25 * normalize(lightPos);
    vec3 offset = vec3(norm.x, 0, norm.z);
    fPosModel = (depthMVP * vec4(aPos - offset, 1.0)).xyz;
    fNor = aNor;
    fTex = aTex;
    layer = int(aLayer);
}
