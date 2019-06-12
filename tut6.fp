#version 330 core
in vec2 uv;
out vec4 FragColor;

uniform sampler2D ourTexture;
uniform sampler2D gratexture;

void main()
{
    vec4 anotherbrickonthewall = texture(ourTexture, uv);
    vec4 leaveuskidsalone = texture(gratexture, uv);
    FragColor = mix(anotherbrickonthewall, leaveuskidsalone, .2);
}