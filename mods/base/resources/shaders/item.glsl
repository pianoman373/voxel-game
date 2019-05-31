#pragma BEGINVERTEX

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

out vec2 fTexCoord;
out vec3 fNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	fTexCoord = vTexCoord;
	fNormal = vNormal;
	gl_Position = vec4(vec2(projection * model * vec4(vPosition, 1.0)), 0.0, 1.0);
}

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

uniform sampler2DArray texArray;
uniform float textureIndex;

in vec2 fTexCoord;
in vec3 fNormal;

out vec4 outColor;

void main() {
    vec3 color = pow(texture(texArray, vec3(fTexCoord.x, fTexCoord.y, textureIndex)).rgb, vec3(2.2)) * dot(normalize(fNormal), normalize(vec3(-0.45, 0.6, 0.25))) * 1.;

    color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, 1.0);
}

#pragma ENDFRAGMENT