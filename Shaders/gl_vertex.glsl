#version 330 core

uniform mat4 matM;
uniform mat4 matrix_ModelToWorld; // model -> world
uniform mat4 matrix_WorldToModel; // world -> model
uniform mat4 matrix_ViewProjection; // world -> clip
uniform mat4 matrix_View; // world -> view
uniform mat4 matrix_Projection; // view -> clip

in vec4 positionOS;
in vec4 normalTS;
in vec4 vertexColor;

out vec4 colf;
out vec3 normalCS;
out vec4 positionCS;

void main ()
{
	vec4 positionWS = matrix_ModelToWorld * positionOS;
	vec4 normalWS = normalTS;
	positionCS = matrix_ViewProjection * positionOS;


	gl_Position = matM * positionOS;
	//gl_Position = positionCS;
	colf = vertexColor;
	normalCS = vertexColor.xyz;
};
