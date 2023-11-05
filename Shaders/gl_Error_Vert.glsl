#version 330 core


uniform mat4 matrix_ModelToWorld; // model -> world
uniform mat4 matrix_WorldToModel; // world -> model
uniform mat4 matrix_ViewProjection; // world -> clip
uniform mat4 matrix_View; // world -> view
uniform mat4 matrix_Projection; // view -> clip

in vec4 positionOS;

out vec4 positionCS;

void main ()
{
	vec4 positionWS = matrix_ModelToWorld * positionOS;
	positionCS = matrix_ViewProjection * positionWS;

	gl_Position = positionCS;

};
