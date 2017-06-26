#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragNormal;

void main()
{
    gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);

    fragNormal = (view * model * vec4(normals, 0.0)).xyz;
}