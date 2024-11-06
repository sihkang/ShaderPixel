#version 410 core

out vec4 fragColor;

uniform vec2 iResolution;
uniform float iTime;
uniform vec3 campos;

float sphereSDF(vec3 pos, vec3 center, float radius) {
    return length(pos - center) - radius;
}

vec3 spheres[3] = vec3[](vec3(0.0, 0.0, 0.0), vec3(2.0, 1.0, -3.0), vec3(-1.5, -0.5, 2.0));
vec3 colors[3] = vec3[](vec3(0.7, 0.2, 0.2), vec3(0.2,0.7,0.2), vec3(0.2,0.2,0.7));
float radii[3] = float[](1.0, 0.5, 0.8);

int idx;

float minSphereSDF(vec3 pos) {
    float minDist = 1e10; // 초기값을 큰 값으로 설정
    for (int i = 0; i < 3; i++) {
        float d = sphereSDF(pos, spheres[i], radii[i]);
		if (minDist > d)
		{
			minDist = d;
			idx = i;
		}
    }
    return minDist;
}

float raymarcher(in vec3 rayOrigin, in vec3 rayDir) {
    const float maxd = 50.0;
    const float eps = 0.01;
    float dist = 0.0;

    for (int i = 0; i < 100; i++) {
        vec3 currentPos = rayOrigin + rayDir * dist;
        float h = minSphereSDF(currentPos); // 최소 SDF 사용
        if (h < eps) break; // 오브젝트에 도달하면 종료
        dist += h;
        if (dist > maxd) return -1.0; // 최대 거리를 넘으면 없음
    }
    return dist;
}

vec3 normal( in vec3 pos )
{
    const float eps = 0.005;

    const vec3 v1 = vec3( 1.0,-1.0,-1.0);
    const vec3 v2 = vec3(-1.0,-1.0, 1.0);
    const vec3 v3 = vec3(-1.0, 1.0,-1.0);
    const vec3 v4 = vec3( 1.0, 1.0, 1.0);

	return -normalize( v1*sphereSDF( pos + v1*eps, spheres[idx], radii[idx] ) + 
					  v2*sphereSDF( pos + v2*eps, spheres[idx], radii[idx] ) + 
					  v3*sphereSDF( pos + v3*eps, spheres[idx], radii[idx] ) + 
					  v4*sphereSDF( pos + v4*eps, spheres[idx], radii[idx] ) );
}

vec3 lightingModel( in vec3 lightdir, in vec3 lightcol, in vec3 albedo, in vec3 norm, in vec3 raydir)
{
	float diffuse = dot(norm, -lightdir);
	float specular = pow(max(dot(-raydir, reflect(lightdir, norm)), -0.0), 128.0);

	return lightcol * (albedo * diffuse + specular);
}

vec3 material(in vec3 contactPoint, in vec3 raydir)
{
	vec3 norm = - normal(contactPoint);

	vec3 lightDir = -normalize(vec3(1.0, 1.0, 1.0));
	vec3 lightCol = vec3(1.0, 1.0, 1.0);

	vec3 albedo = colors[idx];

	float amb = 0.2;
	vec3 pointColor = lightingModel(lightDir, lightCol, albedo, norm, raydir);
	
	return amb + pointColor;
}

vec3 rayrender(vec3 campos, vec3 raydir)
{
	vec3 color = vec3(0.0);

	float dist = raymarcher(campos, raydir);

	if (dist != -1.0)
	{
		vec3 contactPoint = campos + dist * raydir;
		color = material(contactPoint, raydir);
	}
	return color;
}

mat3 calcViewMatrix(in vec3 campos, in vec3 camtar)
{
	vec3 camDir = normalize(camtar - campos);
	vec3 camUp = normalize( cross(camDir, vec3(0.0, 1.0, 0.0)));
	vec3 camRight = normalize( cross(camUp, camDir));
	return mat3(camRight, camUp, camDir);
}

void main()
{
	float time = iTime;

	vec2 pos = 2.0 * gl_FragCoord.xy / iResolution - 1;
	pos.x *= iResolution.x / iResolution.y;

	vec3 camtar = vec3(0.0, 0.0, 0.0);

	mat3 viewMat = calcViewMatrix(campos, camtar);
	vec3 raydir = normalize( viewMat * vec3(pos, 1.0 ));

	vec3 color = rayrender(campos, raydir);
	fragColor = vec4(color, 1.0);
}