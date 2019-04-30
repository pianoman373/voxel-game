#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

out vec3 fPosition;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    fPosition = vPosition;

	mat4 newView = mat4(mat3(view));

    vec4 pos =  projection * newView * vec4(fPosition, 1.0);

    gl_Position = pos.xyww;
}