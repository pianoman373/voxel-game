uniform sampler2D source;
uniform sampler2D gPosition;

uniform float fogInner;
uniform float fogOuter;

vec3 postProcess(vec2 texCoord) {
    vec3 color = texture(source, texCoord).rgb;

    float fogStrength = length(texture(gPosition, texCoord).rgb);
    fogStrength = smoothstep(fogInner, fogOuter, fogStrength);

    vec3 fogColor = vec3(0.7,0.74,0.82);
    fogColor = pow(fogColor, vec3(2.2));

	return mix(color, fogColor, fogStrength);
}