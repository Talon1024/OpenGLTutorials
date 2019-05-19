#version 330 core
in vec2 uv;
in vec3 colour;
out vec4 FragColor;

uniform sampler2D ourTexture;
uniform sampler2D gratexture;

void main()
{
    vec4 anotherbrickonthewall = texture(ourTexture, uv) * vec4(colour, 1.0);
    vec4 leaveuskidsalone = texture(gratexture, vec2(1-uv.s, uv.t));
    FragColor = anotherbrickonthewall + mix(leaveuskidsalone, vec4(0.0,0.0,0.0,1.0), 1-leaveuskidsalone.a);
}