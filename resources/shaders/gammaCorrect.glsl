uniform sampler2D tex;

vec3 postProcess(vec2 texCoord) {
    vec3 color = texture(tex, texCoord).rgb;

    color = pow(color, vec3(1.0/2.2));

    return color;
}