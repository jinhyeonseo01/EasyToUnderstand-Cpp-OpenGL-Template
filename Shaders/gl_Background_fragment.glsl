#version 330 core

out vec4 color;

in vec4 colf;
in vec3 normalCS;
in vec4 positionCS;
in vec2 uv0CS;
in vec2 uv1CS;
in mat3 matrix_TBN;

uniform sampler2D _MainTex;
uniform sampler2D _NormalTex;
uniform sampler2D _AOTex;

uniform vec3 _LightDirection;
uniform vec3 _LightColor;
uniform float _LightPower;

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
	float atten = dot(_LightDirection, normalColor);//normalColor
	atten = min(max(atten, 0), 1);
	//atten = 1 - pow(1 - atten, 3);
	//atten = 1-pow(1-min(max(atten, 0), 1), 30);
	//atten = atten * 0.5 + (1 - 0.5);
	color = vec4(texture(_MainTex, uv0CS).xyz, 1);

	vec3 colorAtt = rgb2hsv(color.xyz);
	float power = 2.0f;
	//colorAtt = vec3(colorAtt.x * (1 - 0.05f * power), colorAtt.y * (1 + 0.05f * power),colorAtt.z  * (1 - 0.0f * power));
	colorAtt = vec3(colorAtt.x + (colorAtt.x > 0.4f ? (0.02f):(-0.02f)), colorAtt.y + 0.2f,colorAtt.z - 0.2f);
	colorAtt = hsv2rgb(colorAtt) * vec3(0.9,0.9,0.8);

	//color = vec4(color.xyz * atten, 1);
	float attenBake = texture(_AOTex, uv1CS).x * 0.4f
	+ texture(_AOTex, uv1CS + vec2(0.001f, 0)).x * 0.15f
	+ texture(_AOTex, uv1CS + vec2(-0.001f, 0)).x * 0.15f
	+ texture(_AOTex, uv1CS + vec2(0, 0.001f)).x * 0.15f
	+ texture(_AOTex, uv1CS + vec2(0, -0.001f)).x * 0.15f;

	float finalAtten = min((pow(1 - atten,1.2f)) * pow(attenBake, 1.2f) + 0.1f, 1);
	

	
	color = vec4(min(1 - pow(1 - color.x, 1.3f), 1), min(1 - pow(1 - color.y, 1.3f), 1), min(1 - pow(1 - color.z, 1.3f), 1), 1);
	color = vec4(mix(colorAtt, color.xyz, finalAtten), 1);


	
	//color = vec4(texture(_AOTex, uv1CS).xyz, 1);

	//color = vec4(mix(color.xyz, color.xyz * vec3(136/255.0f,136/255.0f,199/255.0f), atten), 1);
	//color = vec4(normalColor, 1);
	//color = vec4(pow(colf.x, 2.2f),pow(colf.y, 2.2f),pow(colf.z, 2.2f),1);
	//color = pow(colf, vec4(2.2f,2.2f,2.2f,1));
};