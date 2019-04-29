#version 330 core

const vec2 corners[4] = vec2[4](
    vec2(0.0, 1.0), vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(1.0, 0.0) 
);

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 projection;
uniform vec3 cameraPos;

in float size[1];
in vec3 vColor[1];

out vec2 texCoord;
out vec3 fColor;


//Source: https://stackoverflow.com/questions/17397724/point-sprites-for-particle-system/17400234
void main()
{
    for(int i=0; i<4; ++i)
    {
        vec4 eyePos = gl_in[0].gl_Position;           //start with point position
        eyePos.xy += size[0] * (corners[i] - vec2(0.5)); //add corner position
        gl_Position = projection * eyePos;             //complete transformation
        texCoord = corners[i];                         //use corner as texCoord
        fColor = vColor[0];
        EmitVertex();
    }
}