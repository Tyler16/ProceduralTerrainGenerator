#version 410 core
out vec4 FragColor;
in vec3 TexCoords; 

uniform float uTime;
uniform vec3 lightDir;
uniform vec3 SkyHorizon;

const vec3 SKY_TOP     = vec3(0.0, 0.3, 0.75); 
const vec3 SKY_MID     = vec3(0.2, 0.6, 1.0);  
const vec3 CLOUD_COLOR = vec3(1.0, 1.0, 0.98); 

float hash(vec2 p) { return fract(sin(dot(p, vec2(12.71, 31.17))) * 43758.5453123); }
float vNoise(vec2 p) {
    vec2 i = floor(p); vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i + vec2(0,0)), hash(i + vec2(1,0)), f.x),
               mix(hash(i + vec2(0,1)), hash(i + vec2(1,1)), f.x), f.y);
}

float getClouds(vec2 p) {
    p += uTime * 0.005;
    p.x *= 0.7; // Wind-swept stretch

    // Domain warp: "Smears" the paint
    vec2 warp = vec2(vNoise(p * 0.5), vNoise(p * 0.5 + 1.2));
    vec2 pWarped = p + warp * 0.3;

    // FBM: We use 3 octaves but with higher "lacunarity" 
    // to get that shredded edge look without pixelation.
    float n = vNoise(pWarped * 1.0) * 0.5;
    n += vNoise(pWarped * 2.5) * 0.25;
    n += vNoise(pWarped * 5.5) * 0.125;
    
    // COVERAGE: Lowering the threshold (0.0) ensures 
    // clouds are present even at the zenith.
    float coverage = vNoise(p * 0.6);
    float mask = smoothstep(0.0, 0.4, coverage);
    
    return n * mask;
}

void main() {
    vec3 dir = normalize(TexCoords);
    float h = max(dir.y, 0.0);
    vec3 L = normalize(lightDir);
    float sunIntensity = max(dot(dir, L), 0.0);

    // --- 1. THE GRADIENT ---
    vec3 sky;
    float midPoint = 0.35;
    if (h > midPoint) {
        float blend = (h - midPoint) / (1.0 - midPoint);
        sky = mix(SKY_MID, SKY_TOP, pow(blend, 0.8));
    } else {
        float blend = h / midPoint;
        sky = mix(SkyHorizon, SKY_MID, pow(blend, 1.2));
    }

    // --- 2. THE CLOUDS (Anti-Photorealism) ---
    if (h > 0.01) {
        // Ceiling projection - bias at 0.4 fills the zenith hole
        vec2 cloudUV = (dir.xz / (h + 0.4)) * 3.5; 
        float c = getClouds(cloudUV);
        
        // TIGHTER MASK: A range of only 0.05 (0.45 to 0.5) creates a 
        // "cut-out" painted edge rather than a gaseous realistic one.
        float cloudMask = smoothstep(0.45, 0.5, c); 
        
        // --- 3. CEL-SHADED PAINT BANDS ---
        vec3 shadowCol = mix(SKY_MID, CLOUD_COLOR * 0.75, 0.6);
        vec3 midCol    = mix(shadowCol, CLOUD_COLOR, 0.45);
        
        vec3 cloudPaint;
        // This is the logic that makes it look "Hand-Painted"
        // We force the GPU to use distinct color strips.
        if (c < 0.62) {
            cloudPaint = mix(shadowCol, midCol, smoothstep(0.45, 0.62, c));
        } else {
            cloudPaint = mix(midCol, CLOUD_COLOR, smoothstep(0.62, 0.85, c));
        }

        // Silver Lining (Sun rim light)
        float rim = pow(sunIntensity, 32.0) * cloudMask * 0.7;
        cloudPaint += rim * vec3(1.0, 1.0, 0.9);

        // Integration (No zenith hole multiplier!)
        float horizonFade = smoothstep(0.0, 0.15, h);
        sky = mix(sky, cloudPaint, cloudMask * horizonFade);
    }

    // --- 4. THE CINEMATIC SUN ---
    // Tiny, sharp core disk
    float sunDisk = smoothstep(0.9995, 0.9998, sunIntensity); 
    // LARGE cinematic bloom (Lower power = wider glow)
    float sunGlow = pow(sunIntensity, 256.0) * 0.5;
    // Extra wide "Sky haze" glow
    float sunHaze = pow(sunIntensity, 16.0) * 0.2;
    
    vec3 sunColor = vec3(1.0, 1.0, 0.9) * (sunDisk + sunGlow + sunHaze);

    // God-rays with a safety epsilon for the zenith
    float angle = atan(dir.x, dir.z + 0.00001);
    float rays = sin(angle * 20.0 + uTime * 0.1) * 0.5 + 0.5;
    rays *= pow(sunIntensity, 50.0); 
    
    FragColor = vec4(sky + sunColor + (rays * vec3(1.0, 0.9, 0.8) * 0.1), 1.0);
}

