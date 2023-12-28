#version 330 core

out vec4 color;

in vec4 colf;
in vec3 normalCS;
in vec4 positionCS;
in vec2 uv0CS;
in mat3 matrix_TBN;

uniform sampler2D _MainTex;
uniform sampler2D _NormalTex;
uniform sampler2D _SDFTex;

uniform vec3 _LightDirection;
uniform vec3 _LightColor;
uniform float _LightPower;

in float cosForward;
in float cosRight;
in float angle;

void main ()
{
	color = colf;
	//color = vec4(normalize(normalFrag), 1);
	//float atten = dot(normalize(vec3(1,1,1)), normalize(normalCS));
	//atten = min(max(atten, 0), 1);
	//atten = atten * 0.5 + (1 - 0.5);
	vec3 normalColor = matrix_TBN * normalize(texture(_MainTex, uv0CS).xyz * 2 - 1);
	float atten = dot(_LightDirection, normalColor);//normalColor
	atten = min(max(atten, 0), 1);
	atten = atten * 0.5 + (1 - 0.5);

	float SDFColorR = texture(_SDFTex, uv0CS).x;
	float SDFColorG = texture(_SDFTex, vec2(1 - uv0CS.x, uv0CS.y)).x;
	

	float select = 0;
	float angle2;

	if(angle-1 < 0)
		select = SDFColorG;
	else
		select = SDFColorR;

	if(angle-1 < 0)
		angle2 = 1 - angle;
	else
		angle2 = angle - 1;
	float finalSDFAtten = step(0, cosForward) * step(angle2, select);
	finalSDFAtten = min(max(finalSDFAtten, 0), 1);
	finalSDFAtten = finalSDFAtten * 0.5 + (1 - 0.5);

	color = vec4(texture(_MainTex, uv0CS).xyz, 1);
	color = vec4(color.xyz * finalSDFAtten, 1);
	//color = vec4(SDFColor.xyz, 1);
	//color = vec4(finalSDFAtten,finalSDFAtten,finalSDFAtten,1);
	//color = vec4(normalColor, 1);
	//color = vec4(pow(colf.x, 2.2f),pow(colf.y, 2.2f),pow(colf.z, 2.2f),1);
	//color = pow(colf, vec4(2.2f,2.2f,2.2f,1));
};