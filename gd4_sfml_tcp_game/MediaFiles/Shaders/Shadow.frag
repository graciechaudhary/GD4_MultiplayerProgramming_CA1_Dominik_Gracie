#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D source;
uniform sampler2D depthMap;

void main()
{
    vec4 color = texture(source, TexCoord);
    float depth = texture(depthMap, TexCoord).r;
    
    // Simple shadow calculation based on depth (you can improve this)
    if (depth < 0.5)
    {
        color.rgb *= 0.5;  // Darken the color if the fragment is in shadow
    }

    FragColor = color;
}
