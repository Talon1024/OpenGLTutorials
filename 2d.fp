#version 330 core
in vec2 uv;
out vec4 FragColor;

uniform sampler2D theTexture;

void main()
{
    FragColor = texture(theTexture, uv);
    //FragColor = vec4(uv, 0., 0.25);
}