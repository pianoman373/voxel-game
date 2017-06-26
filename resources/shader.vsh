#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 texCoord;

out vec3 fragNormal;
out vec4 fragColor;
out vec2 uv;
out vec4 fragLightSpace[4];
out vec3 fragPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix[4];

uniform vec3 cameraPos;

void main() {
	fragNormal = (model * vec4(normal, 0.0)).xyz;
	uv = texCoord;
	fragColor = color;

	vec3 globalPos = (model * vec4(position, 1.0)).xyz;

	vec3 dir = normalize(globalPos - vec3(0, 0, 0));

	vec3 newpos = dir * 256;

	vec3 FragPos = vec3(model * vec4(position, 1.0));
	fragPosition = FragPos;

	fragLightSpace[0] = lightSpaceMatrix[0] * vec4(FragPos, 1.0);
	fragLightSpace[1] = lightSpaceMatrix[1] * vec4(FragPos, 1.0);
	fragLightSpace[2] = lightSpaceMatrix[2] * vec4(FragPos, 1.0);
	fragLightSpace[3] = lightSpaceMatrix[3] * vec4(FragPos, 1.0);

	//gl_Position = projection * view * vec4(newpos, 1.0);
	gl_Position = projection * view * model * vec4(position, 1.0);
}
