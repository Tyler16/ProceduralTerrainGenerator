
#version 410 core

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;   // Assuming this is your Sun Direction
uniform vec3 viewPos;
uniform vec3 fogColor;   // This is your SkyHorizon/Fog color
uniform float fogMinDist;
uniform float fogMaxDist;
uniform float uTime;

const vec3 GRASS_SHADOW = vec3(0.25, 0.45, 0.2); 
const vec3 GRASS_MATCHA = vec3(0.35, 0.55, 0.22); 
const vec3 GRASS_SUNNY  = vec3(0.53, 0.7, 0.23); 
const vec3 GLOW_COLOR   = vec3(0.9, 0.95, 0.70); 

const vec3 ROCK_DARK    = vec3(0.35, 0.32, 0.35); 
const vec3 ROCK_LIGHT   = vec3(0.55, 0.52, 0.5); 

float hash(vec2 p) { return fract(sin(dot(p, vec2(12.71, 31.17))) * 43758.5453123); }
float vNoise(vec2 p) {
    vec2 i = floor(p); vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i + vec2(0,0)), hash(i + vec2(1,0)), f.x),
               mix(hash(i + vec2(0,1)), hash(i + vec2(1,1)), f.x), f.y);
}

float brushNoise(vec2 p) {
    p.x *= 0.25; 
    float v = 0.0; float a = 0.5;
    for (int i = 0; i < 3; ++i) {
        v += a * vNoise(p);
        p *= 2.3; a *= 0.5;
    }
    return v;
}

void main() {
    float dist = length(viewPos - FragPos);
    float range = max(fogMaxDist - fogMinDist, 0.1);
    float fadeThreshold = clamp((fogMaxDist - dist) / range, 0.0, 1.0);

    // --- 0. DITHERING ---
    const float bayer[16] = float[](
        0.0/16.0, 8.0/16.0, 2.0/16.0, 10.0/16.0,
        12.0/16.0, 4.0/16.0, 14.0/16.0, 6.0/16.0,
        3.0/16.0, 11.0/16.0, 1.0/16.0, 9.0/16.0,
        15.0/16.0, 7.0/16.0, 13.0/16.0, 5.0/16.0
    );
    int x = int(gl_FragCoord.x) % 4;
    int y = int(gl_FragCoord.y) % 4;
    if (bayer[x + y * 4] > fadeThreshold) discard;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 L = normalize(lightPos); // Sun direction

    // --- 1. BIOME LOGIC ---
    float slope = 1.0 - norm.y;
    float rockJitter = vNoise(FragPos.xz * 0.2) * 0.15;
    float rockMask = smoothstep(0.15 + rockJitter, 0.4 + rockJitter, slope);
    float heightRock = smoothstep(30.0, 40.0, FragPos.y); 
    rockMask = max(rockMask, heightRock);

    // --- 2. TEXTURE LOGIC ---
    float windStrength = uTime * 0.3; 
    vec2 windDir = vec2(windStrength, windStrength * 0.2);
    float wobble = sin(uTime * 0.5) * 0.05;
    vec2 p = (FragPos.xz + windDir) * 0.08;
    float warp = vNoise(p * 0.5 + (uTime * 0.1)); 
    float n = brushNoise(p + warp * 1.2 + wobble);

    float shadowMask = smoothstep(0.05, 0.38, n);
    float sunnyMask  = smoothstep(0.52, 0.95, n);
    vec3 grassAlbedo = mix(mix(GRASS_SHADOW, GRASS_MATCHA, shadowMask), GRASS_SUNNY, sunnyMask);
    vec3 rockAlbedo = mix(ROCK_DARK, ROCK_LIGHT, n);
    vec3 albedo = mix(grassAlbedo, rockAlbedo, rockMask);

    // --- 3. ATMOSPHERIC DEPTH TINTING ---
    // Instead of shadows being black, they turn "Sky Blue" in the distance.
    float shiftFactor = smoothstep(fogMinDist, fogMaxDist * 1.1, dist);
    vec3 skyBlueShadow = fogColor * 0.35; // Cool-toned ambient light
    
    // Mix the base albedo with the sky-tinted shadow. 
    // This makes distant mountains look painterly and integrated.
    vec3 litResult = mix(albedo, albedo + skyBlueShadow, shiftFactor);

    // Add sparkle (only on grass)
    float sparkle = step(0.995, hash(FragPos.xz * 80.0)) * sunnyMask * (1.0 - rockMask);
    litResult += sparkle * 0.3;

    // Rim-Light
    float rim = pow(1.0 - max(dot(viewDir, norm), 0.0), 3.5);
    litResult += rim * GRASS_SUNNY * 0.35;

    // --- 4. SUN-BLEED (Light Wrap) ---
    // If we are looking toward the sun, "bleed" the light over terrain edges.
    float viewTowardSun = max(dot(viewDir, L), 0.0);
    float bleed = pow(viewTowardSun, 8.0) * 0.2; 
    litResult += vec3(1.0, 0.95, 0.8) * bleed;

    // --- 5. FOG ---
    float fogFactor = clamp((fogMaxDist - dist) / range, 0.0, 1.0);
    vec3 finalFog = mix(fogColor, GLOW_COLOR, 0.15);

    FragColor = vec4(mix(finalFog, litResult, fogFactor), 1.0);
}
