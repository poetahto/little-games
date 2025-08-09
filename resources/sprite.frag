#version 420

in vec2 fragTexcoords;
in vec3 fragTint;

out vec4 outColor;

uniform sampler2D spriteTexture;

void main()
{
    outColor = texture(spriteTexture, fragTexcoords) * vec4(fragTint, 1.0);
}
