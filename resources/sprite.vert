#version 420

layout (location = 0) in vec2 clipPosition;
layout (location = 1) in vec2 texcoords;
layout (location = 2) in vec3 tint;

out vec2 fragTexcoords;
out vec3 fragTint;

void main()
{
    gl_Position = vec4(clipPosition, 1.0, 1.0);
    fragTexcoords = texcoords;
    fragTint = tint;
}
