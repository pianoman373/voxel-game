#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


uniform bool doAnimation;

out float size;
out vec3 vColor;

uniform mat4 modelview;

layout(location = 0) in vec4 position;

void main()
{
    gl_Position = view * model * position;
    size = vTexCoord.x;
    vColor = vNormal;
}