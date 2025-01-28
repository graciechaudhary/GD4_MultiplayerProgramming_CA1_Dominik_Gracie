#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

out vec2 TexCoord;

uniform mat4 modelViewProjectionMatrix;

void main()
{
    TexCoord = vec2(inPosition.x, inPosition.y);
    gl_Position = modelViewProjectionMatrix * vec4(inPosition, 1.0);
}