#version 330 core

uniform mat4 modelViewProjectionMatrix;
uniform mat4 lightViewProjectionMatrix;

layout(location = 0) in vec3 vertexPosition;

out vec4 fragPositionLightSpace;

void main()
{
    fragPositionLightSpace = lightViewProjectionMatrix * vec4(vertexPosition, 1.0);
    gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
}