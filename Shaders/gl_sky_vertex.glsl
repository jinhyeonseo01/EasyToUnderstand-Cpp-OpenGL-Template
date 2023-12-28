#version 330 core

uniform mat4 matrix_ModelToWorld; // model -> world
uniform mat4 matrix_WorldToModel; // world -> model
uniform mat4 matrix_ViewProjection; // world -> clip
uniform mat4 matrix_View; // world -> view
uniform mat4 matrix_Projection; // view -> clip

in vec3 positionOS;
in vec2 uv0;

out vec2 uv0CS;

void main ()
{
	vec4 positionWS = matrix_ModelToWorld * vec4(positionOS, 1);
	vec4 positionCS = matrix_ViewProjection * positionWS;


	//normal = texture(normalMap, fs_in.TexCoords).rgb;
    //normal = normalize(normal * 2.0 - 1.0); rgb [0 ~ 1] -> [-1 ~ 1]

	//gl_Position = matM * positionOS;
	gl_Position = positionCS;
	uv0CS = uv0;
};
