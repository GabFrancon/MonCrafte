#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 2) in vec2 aTex;

out vec2 fTex;

void main()
{
    // Output position of the vertex, in clip space map [0..800][0..600] to [-1..1][-1..1]
    // [0..800][0..600] -> [-400..400][-300..300]

    vec2 vertexPosition_homoneneousspace = aPos - vec2(400,300);
    vertexPosition_homoneneousspace /= vec2(400,300);
    gl_Position =  vec4(vertexPosition_homoneneousspace, 0, 1);

    fTex = aTex;
}