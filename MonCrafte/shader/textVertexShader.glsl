#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 2) in vec2 aTex;

uniform vec2 viewport;
uniform vec2 offset;

out vec2 fTex;

void main()
{
    // in clip space, map viewport to [-1..1][-1..1]
    vec2 halfport = viewport / 2;
    vec2 homoneousPos = ( aPos + offset - halfport ) / halfport;
    gl_Position =  vec4(homoneousPos, 0, 1);

    fTex = aTex;
}