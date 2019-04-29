#version 330 core
layout (location = 0) out vec4 outColor;

in vec2 texCoord;
in vec3 fColor;

uniform sampler2D texture0;

void main()
{
    vec4 texColor = texture(texture0, texCoord);
    if (texColor.a < 0.001) {
        discard;
    }
    outColor = vec4(texColor.rgb*fColor, texColor.a);
}