#version 330 core
in vec4 ourPos;
out vec4 FragColor;

void main()
{
    FragColor = ourPos;
}