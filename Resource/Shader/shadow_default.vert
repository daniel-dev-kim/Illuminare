#version 460 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 lightViewProj; // light의 ViewProjection

void main()
{
    gl_Position = lightViewProj * model * vec4(aPos, 1.0);
}