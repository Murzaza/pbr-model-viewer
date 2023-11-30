#version 450 core
out vec4 color;

in vec2 vUV;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, vUV).rgb;
    color = vec4(col, 1.0);
}