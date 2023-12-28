#version 330 core

out vec4 color;

in vec4 colf;
in vec3 normalCS;
in vec4 positionCS;
in vec2 uv0CS;
in mat3 matrix_TBN;

uniform sampler2D _MainTex;
uniform sampler2D _NormalTex;

uniform vec3 _LightDirection;
uniform vec3 _LightColor;
uniform float _LightPower;

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
	atten = 1-pow(1-min(max(atten, 0), 1), 30);
	atten = atten * 0.5 + (1 - 0.5);
	color = vec4(texture(_MainTex, uv0CS).xyz, 1);
	color = vec4(color.xyz * atten, 1);
	//color = vec4(normalColor, 1);
	//color = vec4(pow(colf.x, 2.2f),pow(colf.y, 2.2f),pow(colf.z, 2.2f),1);
	//color = pow(colf, vec4(2.2f,2.2f,2.2f,1));
};