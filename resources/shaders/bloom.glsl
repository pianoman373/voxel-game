uniform sampler2D source;

uniform sampler2D bloom0;
uniform sampler2D bloom1;
uniform sampler2D bloom2;

uniform float bloomStrength;


vec3 postProcess(vec2 texCoord) {
    vec3 color = texture(source, texCoord).rgb;

    color += pow(texture(bloom0, texCoord).rgb, vec3(3))*bloomStrength + pow(texture(bloom1, texCoord).rgb, vec3(3))*bloomStrength + pow(texture(bloom2, texCoord).rgb, vec3(3))*bloomStrength;

    //color = pow(texture(bloom2, texCoord).rgb, vec3(3))*bloomStrength;


	return color;
}