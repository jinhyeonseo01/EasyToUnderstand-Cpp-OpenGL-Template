#version 330 core

uniform mat4 matM;

layout (location = 3) in vec4 positionOS;
//layout (location = 4) in vec3 positionOS2;
layout (location = 8) in vec4 vertexColor;
out vec4 colf;
out vec3 normalFrag;

void main ()
{
	gl_Position = matM * positionOS;// + vec4(positionOS2.xyz, 1);
	colf = vertexColor;
	normalFrag = vertexColor.xyz;
	//normalFrag = (matM * vec4(vertexColor.xyz, 0)).xyz;
};