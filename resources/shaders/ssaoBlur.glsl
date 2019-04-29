uniform sampler2D ssaoInput;
uniform sampler2D source;

vec3 postProcess(vec2 texCoord) {
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, texCoord + offset).r;
        }
    }
    return vec3(result / (4.0 * 4.0)) * texture(source, texCoord).rgb;
}