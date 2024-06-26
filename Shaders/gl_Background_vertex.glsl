#version 330 core

uniform mat4 matrix_ModelToWorld; // model -> world
uniform mat4 matrix_WorldToModel; // world -> model
uniform mat4 matrix_ViewProjection; // world -> clip
uniform mat4 matrix_View; // world -> view
uniform mat4 matrix_Projection; // view -> clip

in vec3 positionOS;
in vec3 normalOS;
in vec3 tangent;
in vec3 bitangent;
in vec4 vertexColor;
in vec2 uv0;
in vec2 uv1;
in vec2 uv2;
in vec2 uv3;
in vec4 boneIds;
in vec4 boneWeights;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec4 colf;
out vec3 normalCS;
out vec4 positionCS;
out vec2 uv0CS;
out vec2 uv1CS;
out mat3 matrix_TBN;

void main ()
{
	vec4 totalPosition = vec4(positionOS, 1.0f);
	vec4 totalNormal = vec4(normalOS,0.0f);
	vec4 totalTangent = vec4(tangent,0.0f);
	vec4 totalBitangent = vec4(bitangent, 0.0f);

	//positionOS = totalPosition.xyz;
	//vec4 positionWS = matrix_ModelToWorld * vec4(positionOS, 1);
	vec4 positionWS = matrix_ModelToWorld * totalPosition;
	vec3 normalWS = 	normalize(vec3(matrix_ModelToWorld * totalNormal));
	vec3 bitangentWS = 	normalize(vec3(matrix_ModelToWorld * totalBitangent));
	vec3 tangentWS = 	normalize(vec3(matrix_ModelToWorld * totalTangent));

	matrix_TBN = mat3(tangentWS, bitangentWS, normalWS);
	//vec3 bitangent = cross(normalOS, tangent);
	positionCS = matrix_ViewProjection * positionWS;

	//normal = texture(normalMap, fs_in.TexCoords).rgb;
    //normal = normalize(normal * 2.0 - 1.0); rgb [0 ~ 1] -> [-1 ~ 1]

	//gl_Position = matM * positionOS;
	gl_Position = positionCS;
	colf = vertexColor;
	normalCS = normalWS;
	uv0CS = uv0;
	uv1CS = uv1;
};
