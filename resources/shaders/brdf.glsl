uniform sampler2D texture0;

#include <lighting>

vec3 postProcess(vec2 texCoord) {
    vec2 integratedBRDF = IntegrateBRDF(texCoord.x, texCoord.y);
    return vec3(integratedBRDF, 0.0);
}