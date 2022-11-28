#version 460 core
out vec4 FragColor;

in vec3 vNormal;
in vec2 vUV;

uniform sampler2D material;

void main()
{
    FragColor = texture(material, vUV);
}