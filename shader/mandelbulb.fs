#version 410 core

out vec4 FragColor;

uniform vec3 cameraPos;    // 카메라 위치
uniform vec3 lightPos;
// uniform vec3 cameraFront; // 카메라의 바라보는 방향
uniform vec2 iResolution;  // 화면 해상도
uniform float iTime;       // 시간

vec3 rotate(vec3 pos, float x, float y, float z) {
    mat3 rotX = mat3(1.0, 0.0, 0.0, 0.0, cos(x), -sin(x), 0.0, sin(x), cos(x));
    mat3 rotY = mat3(cos(y), 0.0, sin(y), 0.0, 1.0, 0.0, -sin(y), 0.0, cos(y));
    mat3 rotZ = mat3(cos(z), -sin(z), 0.0, sin(z), cos(z), 0.0, 0.0, 0.0, 1.0);

    return rotX * rotY * rotZ * pos;
}

float hit(vec3 r) {
    r = rotate(r, iTime, iTime, 0.0);
    vec3 zn = r;
    float rad = 0.0;
    float hit = 0.0;
    float p = 8.0 + 2.0 * sin(iTime + zn.z); // Phase-shift by zn.x
    float d = 1.0;
    
    for (int i = 0; i < 10; i++) {
        rad = length(zn);

        if (rad > 2.0) {
            hit = 0.5 * log(rad) * rad / d;
            break;
        } else {
            float th = atan(length(zn.xy), zn.z);
            float phi = atan(zn.y, zn.x);
            float rado = pow(rad, 8.0);
            d = pow(rad, 7.0) * 7.0 * d + 1.0;

            float sint = sin(th * p);
            zn.x = rado * sint * cos(phi * p);
            zn.y = rado * sint * sin(phi * p);
            zn.z = rado * cos(th * p);
            zn += r;
        }
    }

    return hit;
}

vec3 eps = vec3(0.1, 0.0, 0.0);

void main() {
    vec2 pos = -1.0 + 2.0 * gl_FragCoord.xy / iResolution;    
    pos.x *= iResolution.x / iResolution.y;

    vec3 ro = vec3(pos, 2.0); // 카메라 위치
    vec3 la = vec3(0.0, 0.0, -1.0); // 카메라 목표
    
    vec3 cameraDir = normalize(la - ro);
    vec3 cameraRight = normalize(cross(cameraDir, vec3(0.0, 1.0, 0.0)));
    vec3 cameraUp = normalize(cross(cameraRight, cameraDir));

    vec3 rd = normalize(cameraDir + vec3(pos, 0.0)); // 광선 방향
    float t = 0.0;
    float d = 200.0;

    vec3 r;
    vec3 color = vec3(0.0);

    for (int i = 0; i < 100; i++) {
        if (d > 0.001) {    
            r = ro + rd * t;
            d = hit(r);
            t += d;    
        }
    }

    // normal 구하기. gradient로 방향을 구한다.
    vec3 n = vec3(
        hit(r + eps) - hit(r - eps),
        hit(r + eps.yxz) - hit(r - eps.yxz),
        hit(r + eps.zyx) - hit(r - eps.zyx)
    );

    vec3 mat = vec3(0.4, 0.4, 0.4); 
    vec3 lightCol = vec3(0.1, 0.3, 0.6);
    
    vec3 ldir = normalize(lightPos - r);
    vec3 diff = max(dot(ldir, (n)), 0.0) * lightCol * 60.0;
    
    vec3 R = reflect(ldir, n);
    float spec = pow(max(dot(R, -rd), 0.0), 512.0);
    vec3 specular = lightCol * spec;
    color = (diff + specular) * mat;
    FragColor = vec4(color, 1.0);
}