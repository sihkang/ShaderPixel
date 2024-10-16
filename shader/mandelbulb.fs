#version 330 core
out vec4 FragColor;

uniform vec3 cameraPos;
uniform mat3 cameraOrientation;

float mandelbulbDistance(vec3 pos) {
    // 맨델불브 거리 함수
    vec3 z = pos;
    float dr = 1.0;
    float r = 0.0;
    const int ITERATIONS = 8;
    const float BAILOUT = 2.0;
    const float POWER = 8.0;

    for (int i = 0; i < ITERATIONS; i++) {
        r = length(z);
        if (r > BAILOUT) break;

        // 맨델불브 공식
        float theta = acos(z.z / r);
        float phi = atan(z.y, z.x);
        float zr = pow(r, POWER - 1.0);

        dr = zr * dr * POWER + 1.0;

        float newR = zr * r;
        theta *= POWER;
        phi *= POWER;

        z = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta)) * newR;
        z += pos;
    }

    return 0.5 * log(r) * r / dr;
}

// 레이마칭 함수
float rayMarch(vec3 ro, vec3 rd) {
    float t = 0.0;
    const int MAX_STEPS = 100;
    const float MAX_DISTANCE = 50.0;
    const float SURFACE_DIST = 0.001;

    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 p = ro + rd * t;
        float dist = mandelbulbDistance(p);
        if (dist < SURFACE_DIST) return t; // 표면에 닿음
        t += dist;
        if (t > MAX_DISTANCE) break; // 너무 멀리 나감
    }

    return -1.0; // 맨델불브 표면에 닿지 않음
}

void main() {
    vec2 uv = (gl_FragCoord.xy / vec2(640, 480)) * 2.0 - 1.0;
    vec3 ro = cameraPos; // 카메라 위치
    vec3 rd = normalize(cameraOrientation * vec3(uv, -1.0)); // 레이 방향

    float dist = rayMarch(ro, rd);
    if (dist > 0.0) {
        vec3 hitPos = ro + rd * dist;
        FragColor = vec4(vec3(0.5 + 0.5 * hitPos), 1.0); // 기본 컬러링
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // 배경색
    }
}