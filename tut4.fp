#version 330 core
in vec3 colour;
in vec2 uv;
out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, uv) * vec4(colour, 0.0);
}