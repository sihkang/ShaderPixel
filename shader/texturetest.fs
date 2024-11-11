#version 410 core

out vec4 FragColor;

uniform vec2 iResolution;  // 화면 해상도
uniform float iTime;       // 시간
uniform sampler2D iChannel0;

void main()
{
	
	vec2 uv = (gl_FragCoord.xy / iResolution.xy) * 2.0 - 1.0;
	uv.x *= iResolution.x / iResolution.y;

    vec2 rg = textureLod( iChannel0, (uv+ 0.5) / 256.0, 0. ).yx;
	FragColor = vec4(rg, 0.0, 1.0);
}
