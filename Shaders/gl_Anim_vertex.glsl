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

void main ()
{
	vec4 totalPosition = vec4(0.0f);
	int i = 0;
    for(i = 0; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(int(round(boneIds[i])) == -1) 
            break;
        if(int(round(boneIds[i])) >= MAX_BONES) 
        {
            totalPosition = vec4(positionOS, 1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[int(round(boneIds[i]))] * vec4(positionOS, 1.0f);
        totalPosition += localPosition * boneWeights[i];
        //vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
    }
	if(i == 0)
		totalPosition = vec4(positionOS, 1.0f);
	//positionOS = totalPosition.xyz;
	//vec4 positionWS = matrix_ModelToWorld * vec4(positionOS, 1);
	vec4 positionWS = matrix_ModelToWorld * totalPosition;
	vec4 normalWS = matrix_ModelToWorld * vec4(normalOS, 0); //주의 scale값에 따라 normal에 문제생길 수도 있음.
	vec3 bitangent = cross(normalOS, tangent);
	positionCS = matrix_ViewProjection * positionWS;


	//normal = texture(normalMap, fs_in.TexCoords).rgb;
    //normal = normalize(normal * 2.0 - 1.0); rgb [0 ~ 1] -> [-1 ~ 1]

	//gl_Position = matM * positionOS;
	gl_Position = positionCS;
	colf = vertexColor;
	normalCS = normalOS;
	uv0CS = uv0;
};
