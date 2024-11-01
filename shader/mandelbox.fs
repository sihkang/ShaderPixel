#version 410 core

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec2 iResolution;  // 화면 해상도
uniform float iTime;       // 시간

void sphereFold(inout vec3 z, inout float dz)
{
	float r2 = dot(z,z);
	if (r2 < 0.5)
    { 
		float temp = 2.0;
		z *= temp;
		dz*= temp;
	}
    else if (r2 < 1.0)
    { 
		float temp = 1.0 / r2;
		z *= temp;
		dz*= temp;
	}
}

void boxFold(inout vec3 pos, inout float dz)
{
	pos = clamp(pos, -1.0, 1.0) * 2.0 - pos;
}

float mandelbox(vec3 pos)
{
    float scale = 3.0 + sin(iTime);
	vec3 offset = pos;
	float dr = 1.0;
	for (int n = 0; n < 10; n++)
    {
		boxFold(pos, dr);
		sphereFold(pos, dr);
        pos = scale * pos + offset;
        dr = dr * abs(scale) + 1.0;
	}
	float r = length(pos);
	return r / abs(dr);
}

float raymarcher( in vec3 rayOrigin, in vec3 rayDir )
{
	const float maxd = 50.0;
	const float eps = 0.01;
    float h = eps * 2.0;
    float dist = 0.0;
	
    for( int i = 0; i < 100; i++ )
    {
        if( h < eps || dist > maxd ) break;
	    h = mandelbox( rayOrigin + rayDir * dist );
        dist += h * 1.0;
    }

    if( dist < maxd ) return dist;
    return -1;
}

vec3 normal( in vec3 pos )
{
    const float eps = 0.005;

    const vec3 v1 = vec3( 1.0,-1.0,-1.0);
    const vec3 v2 = vec3(-1.0,-1.0, 1.0);
    const vec3 v3 = vec3(-1.0, 1.0,-1.0);
    const vec3 v4 = vec3( 1.0, 1.0, 1.0);

	return -normalize( v1*mandelbox( pos + v1*eps ) + 
					  v2*mandelbox( pos + v2*eps ) + 
					  v3*mandelbox( pos + v3*eps ) + 
					  v4*mandelbox( pos + v4*eps ) );
}

float ambientOcclusion( in vec3 pos, in vec3 normal )
{
	float occ = 0.0;
    float scaleFactor = 1.0;
    for( int i=0; i<5; i++)
    {
        float offsetDist = 0.01 + 0.12 * float(i) / 4.0;
        vec3 aopos =  normal * offsetDist + pos;
        float distToSurface = mandelbox( aopos );
        occ += -(distToSurface - offsetDist) * scaleFactor;
        scaleFactor *= 0.95;
    }
    return clamp( 1.0 - 3.0 * occ, 0.0, 1.0 );    
}

vec3 lightingModel( in vec3 lightdir, in vec3 lightcol, in vec3 albedo, in vec3 norm, in vec3 camdir )
{    
    float diffuse = pow(0.5 + 0.5*dot(norm, -lightdir),2.0);;
    float specular = pow(max(dot(-camdir, reflect(lightdir, norm)), -0.0), 8.0);
    
    return lightcol * (albedo * diffuse + specular);
}

vec3 material( in vec3 contactPoint , in vec3 camdir )
{    
	vec3 norm = -normal(contactPoint);
    
    vec3 lightDir = -normalize(vec3(5.0,10.0,-20.0));
	vec3 lightCol = vec3(1.0, 0.9, 0.8);
	
    vec3 albedo = vec3(0.32,0.28,0.0);
    
    float ao = ambientOcclusion(contactPoint, norm);
    
    vec3 pointColor = lightingModel(lightDir, lightCol, albedo, norm, camdir);
    
    return 0.5 * ao + 0.5 * (pointColor);
	
}

vec3 rayrender(vec3 campos, vec3 camdir)
{
   vec3 col = vec3(0.0);
    
   float dist = raymarcher(campos, camdir);
    
    if (dist != -1.0) 
    {
    	vec3 contactPoint = campos + dist * camdir;
    	col = material(contactPoint, camdir);
    }
    
    return col;
}

mat3 calcViewMatrix( in vec3 campos, in vec3 camtar, in float radian )
{
    vec3 camDir = normalize( camtar - campos );
    vec3 camUp = normalize( cross(camDir,vec3(sin(radian),cos(radian),0.0)));
    vec3 camRight = normalize( cross(camUp,camDir));
    return mat3( camUp, camRight, camDir );
}

void main()
{
	float time = iTime;

	vec2 pos = 2.0 * gl_FragCoord.xy / iResolution - 1;
	pos.x *= iResolution.x / iResolution.y;

	vec3 campos = vec3(20.0 * cos(time / 5.0), 10.0, 10.0 * sin(time / 5.0));	
	vec3 camtar = vec3(0.0, 0.0, 0.0);

	mat3 viewMat = calcViewMatrix(campos, camtar, 0.0);
	vec3 camdir = normalize( viewMat * vec3(pos, 0.9) );

	vec3 color = rayrender(campos, camdir);
	FragColor = vec4(color, 1.0);
}
