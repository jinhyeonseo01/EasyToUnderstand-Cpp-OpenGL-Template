#version 330 core

out vec4 color;
in vec4 colf;
in vec3 normalFrag;

void main ()
{
	color = colf;
	//color = vec4(normalize(normalFrag), 1);
	float atten = dot(normalize(vec3(1,1,1)), normalize(normalFrag));
	atten = min(max(atten, 0), 1);
	atten = atten * 0.5 + (1 - 0.5);
	color = vec4(vec3(1,1,1) * atten, 1);
	//color = vec4(pow(colf.x, 2.2f),pow(colf.y, 2.2f),pow(colf.z, 2.2f),1);
	//color = pow(colf, vec4(2.2f,2.2f,2.2f,1));
};