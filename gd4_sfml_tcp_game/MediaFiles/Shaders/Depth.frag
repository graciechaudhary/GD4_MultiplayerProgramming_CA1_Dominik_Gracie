#version 330 core

in vec2 TexCoord;

out float FragColor;

uniform sampler2D source;
uniform vec3 lightPosition;

void main()
{
    vec4 sceneColor = texture(source, TexCoord);
    float depth = length(lightPosition - sceneColor.xyz);
    FragColor = depth;
}