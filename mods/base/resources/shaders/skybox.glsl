#pragma BEGINVERTEX

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

out vec3 fPosition;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    fPosition = vPosition;

	mat4 newView = mat4(mat3(view));

    vec4 pos =  projection * newView * vec4(fPosition, 1.0);

    gl_Position = pos.xyww;
}

#pragma ENDVERTEX



#pragma BEGINFRAGMENT

#define PI 3.141592
#define iSteps 16
#define jSteps 8

uniform float bloomStrength;

vec2 rsi(vec3 r0, vec3 rd, float sr) {
    // ray-sphere intersection that assumes
    // the sphere is centered at the origin.
    // No intersection when result.x > result.y
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, r0);
    float c = dot(r0, r0) - (sr * sr);
    float d = (b*b) - 4.0*a*c;
    if (d < 0.0) return vec2(1e5,-1e5);
    return vec2(
        (-b - sqrt(d))/(2.0*a),
        (-b + sqrt(d))/(2.0*a)
    );
}

vec3 atmosphere(vec3 r, vec3 r0, vec3 pSun, float iSun, float rPlanet, float rAtmos, vec3 kRlh, float kMie, float shRlh, float shMie, float g) {
    // Normalize the sun and view directions.
    pSun = normalize(pSun);
    r = normalize(r);

    // Calculate the step size of the primary ray.
    vec2 p = rsi(r0, r, rAtmos);
    if (p.x > p.y) return vec3(0,0,0);
    p.y = min(p.y, rsi(r0, r, rPlanet).x);
    float iStepSize = (p.y - p.x) / float(iSteps);

    // Initialize the primary ray time.
    float iTime = 0.0;

    // Initialize accumulators for Rayleigh and Mie scattering.
    vec3 totalRlh = vec3(0,0,0);
    vec3 totalMie = vec3(0,0,0);

    // Initialize optical depth accumulators for the primary ray.
    float iOdRlh = 0.0;
    float iOdMie = 0.0;

    // Calculate the Rayleigh and Mie phases.
    float mu = dot(r, pSun);
    float mumu = mu * mu;
    float gg = g * g;
    float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

    // Sample the primary ray.
    for (int i = 0; i < iSteps; i++) {

        // Calculate the primary ray sample position.
        vec3 iPos = r0 + r * (iTime + iStepSize * 0.5);

        // Calculate the height of the sample.
        float iHeight = length(iPos) - rPlanet;

        // Calculate the optical depth of the Rayleigh and Mie scattering for this step.
        float odStepRlh = exp(-iHeight / shRlh) * iStepSize;
        float odStepMie = exp(-iHeight / shMie) * iStepSize;

        // Accumulate optical depth.
        iOdRlh += odStepRlh;
        iOdMie += odStepMie;

        // Calculate the step size of the secondary ray.
        float jStepSize = rsi(iPos, pSun, rAtmos).y / float(jSteps);

        // Initialize the secondary ray time.
        float jTime = 0.0;

        // Initialize optical depth accumulators for the secondary ray.
        float jOdRlh = 0.0;
        float jOdMie = 0.0;

        // Sample the secondary ray.
        for (int j = 0; j < jSteps; j++) {

            // Calculate the secondary ray sample position.
            vec3 jPos = iPos + pSun * (jTime + jStepSize * 0.5);

            // Calculate the height of the sample.
            float jHeight = length(jPos) - rPlanet;

            // Accumulate the optical depth.
            jOdRlh += exp(-jHeight / shRlh) * jStepSize;
            jOdMie += exp(-jHeight / shMie) * jStepSize;

            // Increment the secondary ray time.
            jTime += jStepSize;
        }

        // Calculate attenuation.
        vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));

        // Accumulate scattering.
        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;

        // Increment the primary ray time.
        iTime += iStepSize;

    }

    // Calculate and return the final color.
    return iSun * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie);
}

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

uniform DirectionalLight sun;

uniform samplerCube stars;

in vec3 fPosition;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outColor1;

vec3 atmosphere(vec3 dir, vec3 sunDir) {
    float atmosphere = pow(clamp((-(dir.y*2)) / 2.0, 0.0, 1.0), 1);
    vec3 skyColor = vec3(0.2,0.4,0.8)*2;

    float sunset = clamp(-sunDir.y, 0.0, 1.0) * pow(clamp(dot(sunDir, dir), 0.0, 1.0), 5.0);
    vec3 horizonColor = mix(vec3(0.7,0.8,0.9)*2, vec3(1.7,0.8,0.9)*2, 0.0);

    return mix(horizonColor,skyColor,atmosphere) * clamp(-dir.y + 1.0, 0.7, 1.0);
}

void main() {
    vec3 dir = -normalize(fPosition);

    float sunAmount = max( dot( dir, sun.direction ), 0.0 );
    vec3 atmoColor = mix( vec3(0.2,0.4,0.8)*4.0, // bluish
                           vec3(1.0,0.3,0.0)*2.0, // yellowish
                           pow(sunAmount,2.0) );

    vec3 sunLevel = vec3(0.0);

    float theta     = dot(dir, normalize(sun.direction));
    float epsilon   = 0.9995 - 0.999;
    float intensity = clamp((theta - 0.999) / epsilon, 0.0, 1.0);

    sunLevel = vec3(1.0) * intensity;

    atmoColor = atmoColor * clamp(dot(normalize(sun.direction), vec3(0.0f, -1.0f, 0.0f)) , 0, 1);

    vec3 color = mix(atmoColor, vec3(0.1,0.2,0.3), clamp(dot(vec3(0.0, -1.0, 0.0), dir) + 0.2, 0.0, 1.0));





    color = atmosphere(dir, sun.direction);
    
    

    

    color += sunLevel;

    // color = atmosphere(
    //     -dir,           // normalized ray direction
    //     vec3(0,6372e3,0),               // ray origin
    //     -normalize(sun.direction),                        // position of the sun
    //     42.0,                           // intensity of the sun
    //     6371e3,                         // radius of the planet in meters
    //     6471e3,                         // radius of the atmosphere in meters
    //     vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
    //     21e-6,                          // Mie scattering coefficient
    //     8e3,                            // Rayleigh scale height
    //     1.2e3,                          // Mie scale height
    //     0.758                           // Mie preferred scattering direction
    // );

    outColor = vec4(color, 1.0);
    outColor1 = vec4(color * bloomStrength, 1.0);

}

#pragma ENDFRAGMENT