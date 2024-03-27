#version 330 core

out vec4 color;

in vec4 colf;
in vec3 normalCS;
in vec4 positionCS;
in vec2 uv0CS;
in mat3 matrix_TBN;

uniform sampler2D _MainTex;
uniform sampler2D _NormalTex;
uniform sampler2D _MTex;

uniform vec3 _LightDirection;
uniform vec3 _LightColor;
uniform float _LightPower;

uniform mat4 matrix_View; // world -> view

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0f, -1.0f / 3.0f, 2.0f / 3.0f, -1.0f);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
};

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
};

void main ()
{
	color = colf;
	//color = vec4(normalize(normalFrag), 1);
	//float atten = dot(normalize(vec3(1,1,1)), normalize(normalCS));
	//atten = min(max(atten, 0), 1);
	//atten = atten * 0.5 + (1 - 0.5);
	
	vec3 normalColor = normalize(vec3((texture(_NormalTex, uv0CS).xy * 2 - 1), 1));
	normalColor = normalize(matrix_TBN * normalColor);
	float attenDot = dot(_LightDirection, normalColor);//normalColor
	float atten = attenDot;

	float atten2 = pow(1-abs(atten), 8);

	atten = 1-pow(1-min(max(atten, 0), 1), 30);

	//atten = atten * 0.5 + (1 - 0.5);
	color = vec4(texture(_MainTex, uv0CS).xyz, 1);

	vec3 colorAtt = rgb2hsv(color.xyz);
	float power = 2.0f;
	//colorAtt = vec3(colorAtt.x * (1 - 0.1f * power), colorAtt.y * (1 + 0.1f * power),colorAtt.z  * (1 - 0.0f * power));
	colorAtt = vec3(colorAtt.x + (colorAtt.x > 0.28f ? (0.075f):(-0.075f)), colorAtt.y + 0.2f, colorAtt.z - 0.08f);
	colorAtt = hsv2rgb(colorAtt);

	//color = vec4(color.xyz * atten, 1);

	//Real
	//color = vec4(mix(color.xyz, colorAtt, atten), 1);

	vec3 colorAtt2 = rgb2hsv(color.xyz);
	colorAtt2 = vec3(colorAtt2.x + (colorAtt2.x > 0.28f ? (0.1f):(-0.1f)), colorAtt2.y + 0.22f, colorAtt2.z - 0.12f);
	colorAtt2 = hsv2rgb(colorAtt2);

	//Real
	//color = vec4(mix(color.xyz, colorAtt2, atten2), 1);


	//color += dot(vec3(inverse(matrix_View) * vec4(0,0,1,0)), normalColor);

	vec3 viewDirection = vec3(inverse(matrix_View) * vec4(0,0,1,0));

	float rim1 = pow(1-min(max(dot(vec3(inverse(matrix_View) * vec4(0,0,1,0)), normalColor), 0), 1), 3);

	float rim2 = 1 - pow(1 - pow(1-min(max(dot(vec3(inverse(matrix_View) * vec4(0,0,1,0)), normalColor), 0), 1), 8), 20);

	float rim3 = (1 - pow(1 - min(max(attenDot, 0), 1), 7)) * rim2;

	//color += pow(1-min(max(dot(vec3(inverse(matrix_View) * vec4(0,0,1,0)), normalColor), 0), 1), 3);

	vec3 colorAtt3 = rgb2hsv(color.xyz);
	colorAtt3 = vec3(colorAtt3.x + (colorAtt3.x > 0.28f ? (0.09f):(-0.09f)), max(colorAtt3.y - 0.15f, 0), min(colorAtt3.z + 0.5f, 1));
	colorAtt3 = hsv2rgb(colorAtt3);

	//colorAtt3

	//color = vec4(mix(color.xyz, vec3(1,1,1), rim3), 1);//colorAtt3
	
	//Real
	//color = vec4(mix(color.xyz, colorAtt3, rim3), 1);//colorAtt3


	vec3 colorAtt4 = rgb2hsv(color.xyz);
	colorAtt4 = vec3(colorAtt4.x + (colorAtt4.x > 0.28f ? (0.09f):(-0.09f)), max(colorAtt4.y - 0.1f, 0), min(colorAtt4.z + 0.7f, 1));
	colorAtt4 = hsv2rgb(colorAtt4);

	//float spec = pow(min(max(
	//dot((attenDot * 2 * normalColor - _LightDirection), -viewDirection)
	//,0), 1), 20);

	float spec = pow(min(max(
	dot(normalColor, viewDirection)
	,0), 1), 20);
	spec = 1-pow(1-spec,10);
	
	//color += vec4(colorAtt4, 0) * texture(_MTex, uv0CS).z * spec;


	//
	color = vec4(mix(color.xyz, colorAtt, atten), 1);
	color = vec4(mix(color.xyz, colorAtt2, atten2), 1);
	color = vec4(mix(color.xyz, colorAtt3, rim3), 1);//colorAtt3 Rim
	//


	color = vec4(mix(color.xyz, colorAtt4, ((1-atten)*0.8f+0.2f) * texture(_MTex, uv0CS).z * spec), 1);//colorAtt4 Hair Spec
	
	//color += atten2;
	
	//color = vec4(normalColor, 1);
	//color = vec4(pow(colf.x, 2.2f),pow(colf.y, 2.2f),pow(colf.z, 2.2f),1);
	//color = pow(colf, vec4(2.2f,2.2f,2.2f,1));
};
