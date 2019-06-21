#version 330 core
in vec2 uv;
out vec4 gl_FragColor;

uniform sampler2D theTexture;

void main()
{
    gl_FragColor = texture(theTexture, uv);
}