#version 330 core

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;

 void main()
{
  for(int i = 0; i < gl_in.length(); i++)
  {
     // copy attributes
    gl_Position = gl_in[i].gl_Position;
    fragNormal = vec3(0.0, 0.0, 0.0);
    uv = vec2(0.0, 0.0);
    fragPos = gl_Position.xyz;
 
    // done with the vertex
    EmitVertex();
  }
}