#include <lighting>

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gRoughnessMetallic;

vec3 postProcess(vec2 texCoord) {
    vec3 FragPos = texture(gPosition, texCoord).rgb;

    if (length(FragPos) == 0.0) {
        discard;
    }

    // retrieve data from gbuffer
    
    vec3 Albedo = texture(gAlbedo, texCoord).rgb;
	vec4 RoughnessMetallic = texture(gRoughnessMetallic, texCoord);
    float emission = RoughnessMetallic.a;
    
    return emission * Albedo;
}