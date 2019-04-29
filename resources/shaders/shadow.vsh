#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in ivec4 vBoneIDs;
layout (location = 5) in vec4 vBoneWeights;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 viewPos;

    viewPos = projection * view * model * vec4(vPosition, 1.0f);

    gl_Position = viewPos;
}