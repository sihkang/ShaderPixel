#version 410 core

out vec4 FragColor;

uniform vec2 iResolution;  // 화면 해상도
uniform float iTime;       // 시간
uniform sampler2D iChannel0;

mat2 rot(float a) { return mat2(cos(a), sin(a), -sin(a), cos(a)); }

float noise(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    
	f = f * f * (3.0 - 2.0 * f);
	vec2 uv = (p.xy + vec2(37.0, 17.0) * p.z) + f.xy;
	// vec2 rg = vec2(sin(iTime));
    vec2 rg = textureLod( iChannel0, (uv+ 0.5) / 256.0, 0. ).yx;
	return -1.0+2.0*mix( rg.x, rg.y, f.z );
}

float smoke(vec3 pos) 
{
    vec3 q = 2.0 * pos;
    float f = 0.0, amp = 0.5;
    for(int i = 0; i < 5; ++i, amp *= 0.4, q *= 2.1) 
    { // fbm
        q += iTime * vec3(0.17, -0.5, 0);
        f += amp * noise(q);
    }
    float noiseShape = 0.5 + 0.8 * max(pos.y, 0.0) - 1.0 * length(pos.xz);
    return clamp(1.0 +  noiseShape * f - length(pos), 0.0, 1.0);
}


vec3 shading(vec3 rayOrigin, vec3 rayDir) 
{
    vec3 lightDir = normalize(vec3(0.5, 1.0, -0.7));

    const float nbStep = 30.0, diam = 5.0, rayLength = diam / nbStep;

    float start = length(rayOrigin) - diam / 2.0, end = start + diam;
    float sumDen = 0.0, sumDif = 0.0;
    
    for(float d = end; d > start; d -= rayLength) { // raymarching
        vec3 pos = rayOrigin + d * rayDir;
    	if(dot(pos, pos) > diam * diam) break;
        float den = smoke(pos);
        sumDen += den;
        if(den > 0.02) 
			sumDif += max(0.0, den - smoke(pos + lightDir * 0.17));
    }

    const vec3 lightCol = vec3(0.95, 0.75, 0.3);
    float light = 10.0 * pow(max(0.0, dot(rayDir, lightDir)), 10.0);
    vec3 col = 0.01 * light * lightCol;
    col += 0.4 * sumDen * rayLength * vec3(0.8, 0.9, 1.0); // ambient
    col += 1.3 * sumDif * rayLength * lightCol;         // diffuse
	return col;
}

void main()
{
	
	vec2 uv = (gl_FragCoord.xy / iResolution.xy) * 2.0 - 1.0;
	uv.x *= iResolution.x / iResolution.y;
 
    vec3 rayDir = normalize(vec3(uv, 2.0));
    vec3 camPos = vec3(0.0, 1.0, -5.5);
    
	FragColor = vec4(shading(camPos, rayDir), 1.0);
}
