#version 330 core
in vec2 uv;
in vec3 colour;
out vec4 FragColor;

uniform sampler2D brickWall;
uniform sampler2D awesomeFace;
uniform float blendFactor;

void main()
{
    vec4 anotherbrickonthewall = texture(brickWall, uv);
    vec4 leaveuskidsalone = texture(awesomeFace, uv);
    FragColor = mix(anotherbrickonthewall, leaveuskidsalone, blendFactor);
}