#version 330 core
in vec2 uv;
in vec3 colour;
out vec4 FragColor;

uniform sampler2D ourTexture;
uniform sampler2D gratexture;

void main()
{
    // Use alpha from one texture, use RGB from the other
    float alpha = texture(gratexture, uv).a;
    vec4 texel = vec4(texture(ourTexture, uv).rgb, alpha);
    FragColor = mix(texel, vec4(1.0, 0.0, 1.0, 1.0), 1-texel.a);
}