#version 460

uniform sampler2D ParticleTex;

in float Transparency;
in vec2 texCoord;

layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = texture(ParticleTex, texCoord);
    FragColor.a *= Transparency;
}