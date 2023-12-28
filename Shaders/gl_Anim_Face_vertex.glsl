#version 330 core

uniform mat4 matrix_ModelToWorld; // model -> world
uniform mat4 matrix_WorldToModel; // world -> model
uniform mat4 matrix_ViewProjection; // world -> clip
uniform mat4 matrix_View; // world -> view
uniform mat4 matrix_Projection; // view -> clip

uniform vec3 _WorldForward;
uniform vec3 _WorldRight;

uniform vec3 _LightDirection;
uniform vec3 _LightColor;
uniform float _LightPower;

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
out mat3 matrix_TBN;

out float cosForward;
out float cosRight;
out float angle;

void main ()
{
	vec4 totalPosition = vec4(0.0f);
	vec4 totalNormal = vec4(0.0f);
	vec4 totalTangent = vec4(0.0f);
	vec4 totalBitangent = vec4(0.0f);
	int i = 0;
    for(i = 0; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(int(round(boneIds[i])) == -1) 
            break;
        if(int(round(boneIds[i])) >= MAX_BONES) 
        {
            totalPosition = vec4(positionOS, 1.0f);
			totalNormal = vec4(normalOS, 0.0f);
			totalTangent = vec4(tangent, 0.0f);
			totalBitangent = vec4(tangent, 0.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[int(round(boneIds[i]))] * vec4(positionOS, 1.0f);
		vec4 localNormal = finalBonesMatrices[int(round(boneIds[i]))] * vec4(normalOS, 0.0f);
		vec4 localTangent = finalBonesMatrices[int(round(boneIds[i]))] * vec4(tangent, 0.0f);
		vec4 localBitangent = finalBonesMatrices[int(round(boneIds[i]))] * vec4(bitangent, 0.0f);
		

        totalPosition 	+= localPosition 	* boneWeights[i];
		totalTangent 	+= localTangent 	* boneWeights[i];
		totalNormal 	+= localNormal 		* boneWeights[i];
		totalBitangent 	+= localBitangent 	* boneWeights[i];
        //vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
    }
	if(i == 0)
		totalPosition = vec4(positionOS, 1.0f);
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

	cosForward = dot(normalize(_WorldForward), normalize(vec3(_LightDirection.x, 0, _LightDirection.z)));
	cosRight = dot(normalize(_WorldRight), normalize(vec3(_LightDirection.x, 0, _LightDirection.z)));
	angle = (acos(cosRight)/1.570796);


	//gl_Position = matM * positionOS;
	gl_Position = positionCS;
	colf = vertexColor;
	normalCS = normalWS;
	uv0CS = uv0;
};
