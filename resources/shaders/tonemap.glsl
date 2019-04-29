uniform sampler2D source;

vec3 aces(vec3 col, float exposure)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((col*(a*col+b))/(col*(c*col+d)+e), 0.0, 1.0);
}

vec3 reinhard(vec3 col, float exposure) {
    return vec3(1.0) - exp(-col * exposure);
}

vec3 postProcess(vec2 texCoord) {
    vec3 color = texture(source, texCoord).rgb;

    // HDR tonemapping
    const float exposure = 1.0;
    color = aces(color, exposure);

    // float fogStrength = length(texture(gPosition, texCoord).rgb);
    // fogStrength = smoothstep(fogInner, fogOuter, fogStrength);

    // vec3 fogColor = vec3(0.7,0.74,0.82)*1.15;

	return color;
}