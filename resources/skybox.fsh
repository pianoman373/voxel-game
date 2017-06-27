#version 330 core

uniform vec3 sunDirection;
uniform vec3 sunColor;

in vec3 direction;

out vec4 fragColor;

void main() {
    vec3 dir = normalize(direction);

    float sunAmount = max( dot( dir, sunDirection ), 0.0 );
    vec3 atmoColor = mix( vec3(0.5,0.6,1.0), // bluish
                           vec3(1.2,0.9,0.7), // yellowish
                           pow(sunAmount,2.0) );

    vec3 sun = vec3(0.0);

    float theta     = dot(dir, normalize(sunDirection));
    float epsilon   = 0.9995 - 0.999;
    float intensity = clamp((theta - 0.999) / epsilon, 0.0, 1.0);

    sun = sunColor * intensity;



    vec3 color = mix(atmoColor, vec3(0.1,0.2,0.3), clamp(dot(vec3(0.0, 1.0, 0.0), dir), 0.0, 1.0));
    color += sun;

    fragColor = vec4(color, 1.0);
}
