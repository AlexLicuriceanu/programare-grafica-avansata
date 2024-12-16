#version 330

in vec2 texCoord;

uniform sampler2D sceneTexture;

out vec4 FragColor;

void main()
{
    vec3 color = texture(sceneTexture, texCoord).rgb;
    FragColor = vec4(vec3(0, 0, 0.1), 1);
}
