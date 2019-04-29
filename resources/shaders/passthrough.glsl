uniform sampler2D texture0;

vec3 postProcess(vec2 texCoord) {
    return texture(texture0, texCoord).rgb;
}