#version 330 core
layout (location = 0) out vec4 outColor;

in vec3 fragPos;
in vec4 screenPos;
in vec3 fragNormal;
in vec3 uv;
in vec3 fragColor;
in float ao;

uniform sampler2D gPosition;
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;

void main()
{
  vec3 color = vec3(0.3, 0.3, 1.0)*0.3;

  outColor = vec4(color, 0.7);
}