
#include "total_include.h"


#pragma region GL_Shader

// #pragma comment(lib,"./StudyDLL.lib")
// $(SolutionDir)3rd Party\lib $(SolutionDir)3rd Party\include
// glew32.lib freeglut.lib assimp-vc143-mt.lib SOIL.lib


class AABB
{
public:
    glm::vec3 min = glm::vec3(-0.01f, -0.01f, -0.01f);
    glm::vec3 max = glm::vec3(0.01f, 0.01f, 0.01f);
    AABB(glm::vec3 min, glm::vec3 max)
    {
        this->min.x = max.x >= min.x ? min.x : max.x;
        this->max.x = max.x < min.x ? min.x : max.x;

        this->min.y = max.y >= min.y ? min.y : max.y;
        this->max.y = max.y < min.y ? min.y : max.y;

        this->min.z = max.z >= min.z ? min.z : max.z;
        this->max.z = max.z < min.z ? min.z : max.z;
    }
    AABB(glm::vec2 min, glm::vec2 max)
    {
        this->min.x = max.x >= min.x ? min.x : max.x;
        this->max.x = max.x < min.x ? min.x : max.x;

        this->min.y = max.y >= min.y ? min.y : max.y;
        this->max.y = max.y < min.y ? min.y : max.y;

        this->min.z = 0.0f;
        this->max.z = 0.0f;
    }
    AABB(glm::vec3 pos, glm::vec3 minSize, glm::vec3 maxSize)
    {
        this->min = pos - minSize;
        this->min = pos + maxSize;

        this->min.x = max.x >= min.x ? min.x : max.x;
        this->max.x = max.x < min.x ? min.x : max.x;

        this->min.y = max.y >= min.y ? min.y : max.y;
        this->max.y = max.y < min.y ? min.y : max.y;

        this->min.z = max.z >= min.z ? min.z : max.z;
        this->max.z = max.z < min.z ? min.z : max.z;
    }
    AABB(glm::vec2 pos, glm::vec2 minSize, glm::vec2 maxSize)
    {
        this->min = glm::vec3(pos - minSize, 0);
        this->max = glm::vec3(pos + maxSize, 0);

        this->min.x = max.x >= min.x ? min.x : max.x;
        this->max.x = max.x < min.x ? min.x : max.x;

        this->min.y = max.y >= min.y ? min.y : max.y;
        this->max.y = max.y < min.y ? min.y : max.y;

        this->min.z = max.z >= min.z ? min.z : max.z;
        this->max.z = max.z < min.z ? min.z : max.z;
    }
    static bool Check(AABB& a, AABB& b)
    {
        if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
        if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
        if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
        return true;
    }
    bool Check(AABB& other)
    {
        return AABB::Check(*this, other);
    }
};




class Material
{
public:
    Shader* shader;
    std::shared_ptr<UniformStorage> uniformStorage;

    bool para_zTest = true;
    bool para_zWrite = true;
    bool para_stencil = false;
    bool para_culling = true;
    bool para_transparent = false;

    unsigned int para_prevAlpha = GL_ONE_MINUS_SRC_ALPHA;
    unsigned int para_nextAlpha = GL_SRC_ALPHA;
    unsigned int para_cullingFace = GL_BACK; //GL_FRONT(기본), GL_BACK, GL_FORNT_AND_BACK
    unsigned int para_cullingCycle = GL_CCW; //GL_CCW(기본) GL_CW
    unsigned int para_zTestType = GL_LEQUAL;
    unsigned int para_stencilType = GL_ALWAYS;
    unsigned int para_stencilMask = 0x00; //0x00~0xFF
    unsigned int para_stencil_sfail = GL_KEEP;
    unsigned int para_stencil_dpfail = GL_KEEP;
    unsigned int para_stencil_dppass = GL_KEEP;

    //GL_ALWAYS	    depth test가 항상 통과됩니다.
    //GL_NEVER	    depth test가 절대 통과되지 않습니다.
    //GL_LESS	    fragment의 깊이 값이 저장된 깊이 값보다 작을 경우 통과시킵니다. (기본)
    //GL_EQUAL	    fragment의 깊이 값이 저장된 깊이 값과 동일한 경우 통과시킵니다.
    //GL_LEQUAL	    fragment의 깊이 값이 저장된 깂이 값과 동일하거나 작을 경우 통과시킵니다.
    //GL_GREATER	fragment의 깊이 값이 저장된 깊이 값보다 클 경우 통과시킵니다.
    //GL_NOTEQUAL	fragment의 깊이 값이 저장된 깊이 값과 동일하지 않을 경우 통과시킵니다.
    //GL_GEQUAL	    fragment의 깊이 값이 저장된 깊이 값과 동일하거나 클 경우 통과시킵니다.

    //스텐실
    //GL_KEEP	현재 저장된 stencil 값을 유지 (기본)
    //GL_ZERO	stencil 값을 0으로 설정
    //GL_REPLACE	stencil 값을 glStencilFunc 함수에서 지정한 레퍼런스 값으로 설정
    //GL_INCR	최댓값보다 작다면 stencil 값을 1만큼 증가시킴
    //GL_INCR_WRAP	GL_INCR와 같지만 최댓값을 초과하면 0로 돌아옴
    //GL_DECR	최솟값보다 크다면 stencil 값을 1만큼 감소시킴
    //GL_DECR_WRAP	GL_DECR와 같지만 0보다 작다면 최댓값으로 설정함
    //GL_INVERT	현재 stencil buffer 값의 비트를 뒤집음

    Material(Shader* shader);
    void SetShader(Shader* shader);
    unsigned int drawType = GL_TRIANGLES;
    //void IndexRender(ModelInfo* model);
    //void ArrayRender(ModelInfo* model);
    //Material* Render(ModelInfo* model);
};

class AttributeSegment
{
public:
    /*
    * Count == 데이터 갯수
    * Size = Count * sizeof(type) 실제 데이터 크기
    */
    char* name = nullptr;
    int segmentCount = -1;

    std::weak_ptr<void> rawBuffer;
    int offsetCount = -1;
    int blockCount = -1;
    unsigned int rawType = GL_FLOAT;
    unsigned int type = GL_FLOAT;
    unsigned int typeSize = -1;

    int location = -1;
    int col = 0;
    int row = 0;
    unsigned int normalized = GL_FALSE;
    
    AttributeSegment(const char* name, int segmentCount);
    AttributeSegment(const char* name, int location, int col, int row, unsigned int rawType, unsigned int type, unsigned int normalized);
    AttributeSegment(const AttributeSegment& origin);
    AttributeSegment(AttributeSegment&& other);
    AttributeSegment& operator =(const AttributeSegment& other);
    AttributeSegment& operator =(AttributeSegment&& other);
    AttributeSegment& ConvertRealSegment(std::shared_ptr<void> rawBuffer, unsigned int type, int offsetCount, int blockCount);
    void Destroy();
    ~AttributeSegment();
};

class UniformSegment
{
public:
    char* name = nullptr;

    int location = -1;
    int shaderID = -1;
    glm::i32vec4 data_vector_int = glm::i32vec4(0,0,0,0);
    glm::vec4 data_vector = glm::vec4(0, 0, 0, 0);
    glm::mat2 data_matrix2 = glm::mat2(1.0f);
    glm::mat3 data_matrix3 = glm::mat3(1.0f);
    glm::mat4 data_matrix4 = glm::mat4(1.0f);

    int col = 1;
    int row = 1;
    unsigned int type = GL_FLOAT;
    unsigned int normalized = GL_FALSE;

    int BindShader(Shader* shader);

    UniformSegment* SetData(float data)
    {
        data_vector = glm::vec4(data, 0, 0, 0);
        data_vector_int = glm::i32vec4(data, 0, 0, 0);
        return this;
    }
    UniformSegment* SetData(int data)
    {
        data_vector = glm::vec4(data, 0, 0, 0);
        data_vector_int = glm::i32vec4(data, 0, 0, 0);
        return this;
    }
    UniformSegment* SetData(glm::vec2 data)
    {
        data_vector = glm::vec4(data, 0, 0);
        data_vector_int = glm::vec4(data, 0, 0);
        return this;
    }
    UniformSegment* SetData(glm::i32vec2 data)
    {
        data_vector = glm::vec4(data, 0, 0);
        data_vector_int = glm::vec4(data, 0, 0);
        return this;
    }
    UniformSegment* SetData(glm::vec3 data)
    {
        data_vector = glm::vec4(data, 0);
        data_vector_int = glm::i32vec4(data, 0);
        return this;
    }
    UniformSegment* SetData(glm::i32vec3 data)
    {
        data_vector = glm::vec4(data, 0);
        data_vector_int = glm::i32vec4(data, 0);
        return this;
    }
    UniformSegment* SetData(glm::vec4 data)
    {
        data_vector = data;
        data_vector_int = data;
        return this;
    }
    UniformSegment* SetData(glm::i32vec4 data)
    {
        data_vector = data;
        data_vector_int = data;
        return this;
    }
    UniformSegment* SetData(glm::mat2 data)
    {
        data_matrix2 = data;
        return this;
    }
    UniformSegment* SetData(glm::mat3 data)
    {
        data_matrix3 = data;
        return this;
    }
    UniformSegment* SetData(glm::mat4 data)
    {
        data_matrix4 = data;
        return this;
    }

    UniformSegment(const char* name, unsigned int type);
    UniformSegment(const UniformSegment& origin);
    UniformSegment& operator =(const UniformSegment& origin);

    UniformSegment& operator =(UniformSegment&& origin);
    UniformSegment(UniformSegment&& origin);
    void UpdateUniformSegment(UniformSegment& segment);
    void Bind();
    ~UniformSegment();
};

class UniformStorage
{
public:
    std::vector<UniformSegment> uniformList;

    UniformStorage()
    {

    }
    UniformStorage(std::vector<UniformSegment> uniformList)
    {
        for (int i = 0; i < uniformList.size(); i++)
            this->PushUniform(uniformList[i]);
    }

    void BindShader(Shader* shader);

    UniformSegment* GetUniform(const char* name)
    {
        for (int i = 0; i < uniformList.size(); i++)
        {
            if (strcmp(uniformList[i].name, name) == 0)
                return &uniformList[i];
        }
        return nullptr;
    }
    void SetUniformData(const char* name, float data)
    {
        UniformSegment* uniform = GetUniform(name);
        if (uniform != nullptr)
            uniform->SetData(data);
    }
    void SetUniformData(const char* name, int data)
    {
        UniformSegment* uniform = GetUniform(name);
        if (uniform != nullptr)
            uniform->SetData(data);
    }
    void SetUniformData(const char* name, glm::vec2 data)
    {
        UniformSegment* uniform = GetUniform(name);
        if (uniform != nullptr)
            uniform->SetData(data);
    }
    void SetUniformData(const char* name, glm::vec3 data)
    {
        UniformSegment* uniform = GetUniform(name);
        if (uniform != nullptr)
            uniform->SetData(data);
    }
    void SetUniformData(const char* name, glm::vec4 data)
    {
        UniformSegment* uniform = GetUniform(name);
        if (uniform != nullptr)
            uniform->SetData(data);
    }
    void SetUniformData(const char* name, glm::i32vec2 data)
    {
        UniformSegment* uniform = GetUniform(name);
        if (uniform != nullptr)
            uniform->SetData(data);
    }
    void SetUniformData(const char* name, glm::i32vec3 data)
    {
        UniformSegment* uniform = GetUniform(name);
        if (uniform != nullptr)
            uniform->SetData(data);
    }
    void SetUniformData(const char* name, glm::i32vec4 data)
    {
        UniformSegment* uniform = GetUniform(name);
        if (uniform != nullptr)
            uniform->SetData(data);
    }

    void PushUniform(UniformSegment* segment)
    {
        auto p_segment = segment;
        PushUniform(*p_segment);
    }
    bool UpdateUniform(UniformSegment* segment)
    {
        auto p_segment = segment;
        UpdateUniform(*segment);
    }
    void PushUniform(UniformSegment&& segment)
    {
        PushUniform(segment);
    }
    bool UpdateUniform(UniformSegment&& segment)
    {
        UpdateUniform(segment);
    }
    void PushUniform(UniformSegment& segment)
    {
        int index = -1;
        for (int i = 0; i < uniformList.size(); i++)
        {
            if (strcmp(uniformList[i].name, segment.name) == 0)
                index = i;
        }
        if (index == -1)
        {
            uniformList.push_back(UniformSegment(segment.name, segment.type));
            index = uniformList.size() - 1;
        }
        uniformList[index].UpdateUniformSegment(segment);
    }
    bool UpdateUniform(UniformSegment& segment)
    {
        int index = -1;
        for (int i = 0; i < uniformList.size(); i++)
        {
            if (strcmp(uniformList[i].name, segment.name) == 0)
                index = i;
        }
        if (index != -1)
        {
            uniformList[index].UpdateUniformSegment(segment);
            return true;
        }

        return false;
    }
    void PushUniforms(UniformStorage& otherStorage)
    {
        for (int i = 0; i < otherStorage.uniformList.size(); i++)
            PushUniform(otherStorage.uniformList[i]);
    }
    void UpdateUniforms(UniformStorage& otherStorage)
    {
        for (int i = 0; i < otherStorage.uniformList.size(); i++)
            UpdateUniform(otherStorage.uniformList[i]);
    }
};


class Loader
{
public:

    static std::shared_ptr<aiScene> ModelNotUV(const char* path, Assimp::Importer& importer)
    {
        aiScene* pScene = (aiScene*)importer.ReadFile(path,
            aiProcess_Triangulate | // 4각형 5각형을 3각형으로
            //aiProcess_GenSmoothNormals | // Normal이 없으면 Smmoth Normal 생성
            aiProcess_GenNormals | // Normal이 없으면 Normal 생성
            //aiProcess_SplitLargeMeshes // 매쉬가 너무 클때 쪼개는거 매쉬 클때 렌더링 유리.
            //aiProcess_ImproveCacheLocality | 삼각형 개선. 잘 되면 켜보기
            aiProcess_GenUVCoords | // UV없으면 UV 계산하게[ 시키기
            //aiProcess_Debone | 손실없이 뼈 제거. 걍 쓰지말자.
            //aiProcess_MakeLeftHanded | // 왼손 좌표계로 변경
            //aiProcess_RemoveComponent | // (animations, materials, light sources, cameras, textures, vertex components 제거
            //aiProcess_PreTransformVertices | // root Node를 제외한 모든 하위 노드들 전부 평탄화. 계층 제거.
            //aiProcess_ValidateDataStructure | // 연결 유효성 검사
            //aiProcess_ImproveCacheLocality | // 캐시히트율을 위해 삼각형 재정렬함.
            //aiProcess_RemoveRedundantMaterials | // 중복이나 안쓰는거 제거
            //aiProcess_FixInfacingNormals | //잘못 연결되서 고장난 노멀 재대로 수정
            //aiProcess_FindDegenerates | //삼각형에서 점이 겹쳐버리면 라인이나 점이 되버리는데, 이걸 Line이나 Point로 변환하는거임. 안쓰는게 나음.
            aiProcess_FindInvalidData | //유효하지 않는 법선벡터, UV를 제거함. 이렇게 제거하고 나면 aiProcess_GenNormals같은게 새롭게 생성해줄거임. 애니메이션에서도 이점이 있다고함.
            aiProcess_GenUVCoords  | //UV를 자체적으로 계산함. 모델링툴에서 생성하는걸 추천하고, UV가 없으면 새롭게 생성하는거임.
            //aiProcess_FindInstances | 너무 매쉬가 많을때 키나봄. 느리다는거같음. 같은 재질인 매쉬들을 하나로 합쳐버리는 기능인듯.
            //aiProcess_OptimizeMeshes |// 매쉬 를 줄여주는 최적화 옵션인듯. aiProcess_OptimizeGraph랑 같이 쓰는게 좋고, #aiProcess_SplitLargeMeshes and #aiProcess_SortByPType.랑 호환됨.
            //걍 안키는게 나을듯. 뭔가 밑에 옵션이랑 호환되는 모양인데, 밑에 옵션을 못씀.
            //aiProcess_OptimizeGraph |//필요없는 노드를 삭제함. 노드가 태그로 쓰일때 누락되는 문제가 잇나봄, 안키는게 나을듯. 계층구조가 손실된다고 함.
            //aiProcess_FlipWindingOrder | CW, CCW 바꾸는거임.
            aiProcess_TransformUVCoords | //UV에 대해서 변환처리 한다고 하는거같음. 텍스쳐 이상해지면 꺼버리도록
            aiProcess_FlipUVs | // 말그대로 uv의 y축을 뒤집음. 그리고 bitangent도 뒤집음.
            aiProcess_JoinIdenticalVertices | // 인덱스 버퍼 기반으로 변환
            aiProcess_CalcTangentSpace | // 탄젠트 계산
            aiProcess_SortByPType // 폴리곤을 타입별로 재정렬함. aiProcess_Triangulate 쓰면 어차피 삼각형만 남아서 필요 없음. 일단 넣어~
        );
        return std::shared_ptr<aiScene>(pScene);
    }
    static std::shared_ptr<aiScene> ModelLoad(const char* path, Assimp::Importer& importer)
    {
        //"./Models/Klee/Avatar_Loli_Catalyst_KleeCostumeWitch.fbx"
        NormalLog(std::cerr) << "Model File Load - " << path << "\n";
        aiScene* pScene = (aiScene*)importer.ReadFile(path,
            aiProcess_Triangulate | // 4각형 5각형을 3각형으로
            //aiProcess_GenSmoothNormals | // Normal이 없으면 Smmoth Normal 생성
            aiProcess_GenNormals | // Normal이 없으면 Normal 생성
            //aiProcess_SplitLargeMeshes // 매쉬가 너무 클때 쪼개는거 매쉬 클때 렌더링 유리.
            //aiProcess_ImproveCacheLocality | 삼각형 개선. 잘 되면 켜보기
            aiProcess_GenUVCoords | // UV없으면 UV 계산하게[ 시키기
            //aiProcess_Debone | 손실없이 뼈 제거. 걍 쓰지말자.
            //aiProcess_MakeLeftHanded | // 왼손 좌표계로 변경
            //aiProcess_RemoveComponent | // (animations, materials, light sources, cameras, textures, vertex components 제거
            //aiProcess_PreTransformVertices | // root Node를 제외한 모든 하위 노드들 전부 평탄화. 계층 제거.
            //aiProcess_ValidateDataStructure | // 연결 유효성 검사
            //aiProcess_ImproveCacheLocality | // 캐시히트율을 위해 삼각형 재정렬함.
            //aiProcess_RemoveRedundantMaterials | // 중복이나 안쓰는거 제거
            //aiProcess_FixInfacingNormals | //잘못 연결되서 고장난 노멀 재대로 수정
            //aiProcess_FindDegenerates | //삼각형에서 점이 겹쳐버리면 라인이나 점이 되버리는데, 이걸 Line이나 Point로 변환하는거임. 안쓰는게 나음.
            aiProcess_FindInvalidData | //유효하지 않는 법선벡터, UV를 제거함. 이렇게 제거하고 나면 aiProcess_GenNormals같은게 새롭게 생성해줄거임. 애니메이션에서도 이점이 있다고함.
            //aiProcess_GenUVCoords  | //UV를 자체적으로 계산함. 모델링툴에서 생성하는걸 추천하고, UV가 없으면 새롭게 생성하는거임.
            //aiProcess_FindInstances | 너무 매쉬가 많을때 키나봄. 느리다는거같음. 같은 재질인 매쉬들을 하나로 합쳐버리는 기능인듯.
            //aiProcess_OptimizeMeshes |// 매쉬 를 줄여주는 최적화 옵션인듯. aiProcess_OptimizeGraph랑 같이 쓰는게 좋고, #aiProcess_SplitLargeMeshes and #aiProcess_SortByPType.랑 호환됨.
            //걍 안키는게 나을듯. 뭔가 밑에 옵션이랑 호환되는 모양인데, 밑에 옵션을 못씀.
            //aiProcess_OptimizeGraph |//필요없는 노드를 삭제함. 노드가 태그로 쓰일때 누락되는 문제가 잇나봄, 안키는게 나을듯. 계층구조가 손실된다고 함.
            //aiProcess_FlipWindingOrder | CW, CCW 바꾸는거임.
            //aiProcess_TransformUVCoords | //UV에 대해서 변환처리 한다고 하는거같음. 텍스쳐 이상해지면 꺼버리도록
            aiProcess_FlipUVs | // 말그대로 uv의 y축을 뒤집음. 그리고 bitangent도 뒤집음.
            aiProcess_JoinIdenticalVertices | // 인덱스 버퍼 기반으로 변환
            aiProcess_CalcTangentSpace | // 탄젠트 계산
            aiProcess_SortByPType // 폴리곤을 타입별로 재정렬함. aiProcess_Triangulate 쓰면 어차피 삼각형만 남아서 필요 없음. 일단 넣어~
        );
        
        return std::shared_ptr<aiScene>(pScene);
    }
    static std::vector<std::shared_ptr<ModelInfo2>> ConvertModelToModelDatas(std::shared_ptr<Model> model, std::shared_ptr<aiScene> aiScene);
};


#define BoneMax 4
#define BoneMaxCount 200
class MeshVertex
{
public:
    glm::vec3 pos = glm::vec3(0, 0, 0);
    glm::vec3 normal = glm::normalize(glm::vec3(1, 1, 1));
    glm::vec3 tangent = glm::vec3(0, 1, 0);
    glm::vec4 colors[8] = { glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1) };
    glm::vec2 uvs[8] = { glm::vec2(0, 0),glm::vec2(0, 0),glm::vec2(0, 0),glm::vec2(0, 0),glm::vec2(0, 0),glm::vec2(0, 0),glm::vec2(0, 0),glm::vec2(0, 0) };
    int colorCount = 0;
    int uvCount = 0;
    int boneIds[BoneMax] = { -1, -1 , -1 ,-1 };
    float boneWeights[BoneMax] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class MeshTexture
{
public:
    unsigned int id;
    std::string type;
};
struct BoneInfo
{
    int id;
    glm::mat4 offset = glm::mat4(1.0f);;
};

class Model
{
public:
    std::map<std::string, BoneInfo> boneInfoMap; //
    int boneCounter = 0;
    std::vector<std::shared_ptr<Mesh>> meshList;
    std::vector<std::shared_ptr<Animation>> animationList;

    int GetBoneID(aiBone* bone)
    {
        return GetBoneID(std::string(bone->mName.C_Str()), bone);
    }
    int GetBoneID(std::string boneName, aiBone* bone)
    {
        //std::string boneName = mesh->mBones[j]->mName.C_Str();
        int boneID = -1;
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCounter;
            //이부분 나중에 다이렉트로 바뀌면 같이 수정해야함.
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(bone->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCounter;
            boneCounter++;
        }
        else
        {
            boneID = boneInfoMap[boneName].id;
        }
        return boneID;
    }
    static std::shared_ptr<Model> processModel(std::shared_ptr<aiScene> scene);
    std::shared_ptr<ModelInfo2> processData();
};

struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

class Bone
{
public:
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    
    int m_NumPositions = 0;
    int m_NumRotations = 0;
    int m_NumScales = 0;

    glm::mat4 m_LocalTransform = glm::mat4(1.0f);
    std::string m_Name;
    int m_ID;

    Bone(const std::string& name, int ID, const aiNodeAnim* channel)
    {
        this->m_Name = name;
        this->m_ID = ID;
        this->m_LocalTransform = glm::mat4(1.0f);

        m_NumPositions = channel->mNumPositionKeys;

        for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
            data.timeStamp = timeStamp;
            m_Positions.push_back(data);
        }

        m_NumRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
            data.timeStamp = timeStamp;
            m_Rotations.push_back(data);
        }

        m_NumScales = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < m_NumScales; ++keyIndex)
        {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyScale data;
            data.scale = AssimpGLMHelpers::GetGLMVec(scale);
            data.timeStamp = timeStamp;
            m_Scales.push_back(data);
        }
    }

    void Update(float animationTime)
    {
        glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation = InterpolateRotation(animationTime);
        glm::mat4 scale = InterpolateScale(animationTime);
        m_LocalTransform = translation * rotation * scale;
    }


    glm::mat4 GetLocalTransform() { return m_LocalTransform; }
    std::string GetBoneName() const { return m_Name; }
    int GetBoneID() { return m_ID; }


    int GetPositionIndex(float animationTime)
    {
        for (int index = 0; index < m_NumPositions - 1; ++index)
        {
            if (animationTime <= m_Positions[index + 1].timeStamp)
                return index;
        }
        assert(0);
    }

    /* Gets the current index on mKeyRotations to interpolate to based on the
    current animation time*/
    int GetRotationIndex(float animationTime)
    {
        for (int index = 0; index < m_NumRotations - 1; ++index)
        {
            if (animationTime <= m_Rotations[index + 1].timeStamp)
                return index;
        }
        assert(0);
    }

    /* Gets the current index on mKeyScalings to interpolate to based on the
    current animation time */
    int GetScaleIndex(float animationTime)
    {
        for (int index = 0; index < m_NumScales - 1; ++index)
        {
            if (animationTime <= m_Scales[index + 1].timeStamp)
                return index;
        }
        assert(0);
    }

    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    /*figures out which position keys to interpolate b/w and performs the interpolation
    and returns the translation matrix*/
    glm::mat4 InterpolatePosition(float animationTime)
    {
        if (1 == m_NumPositions)
            return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

        int p0Index = GetPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
            m_Positions[p1Index].timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position,
            m_Positions[p1Index].position, scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    /*figures out which rotations keys to interpolate b/w and performs the interpolation
    and returns the rotation matrix*/
    glm::mat4 InterpolateRotation(float animationTime)
    {
        if (1 == m_NumRotations)
        {
            auto rotation = glm::normalize(m_Rotations[0].orientation);
            return glm::toMat4(rotation);
        }

        int p0Index = GetRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
            m_Rotations[p1Index].timeStamp, animationTime);
        glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation,
            m_Rotations[p1Index].orientation, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }

    /*figures out which scaling keys to interpolate b/w and performs the interpolation
    and returns the scale matrix*/
    glm::mat4 InterpolateScale(float animationTime)
    {
        if (1 == m_NumScales)
            return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

        int p0Index = GetScaleIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
            m_Scales[p1Index].timeStamp, animationTime);
        glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale
            , scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
};

struct AssimpNodeData
{
    glm::mat4 transformation = glm::mat4(1.0f);
    std::string name;
    int childrenCount = 0;
    std::vector<AssimpNodeData> children;
};

class Animation
{
public:
    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;

    Animation(std::shared_ptr<Model> model, aiScene* scene, aiAnimation* animation)
    {
        m_Duration = animation->mDuration;
        m_TicksPerSecond = animation->mTicksPerSecond;
        ReadHeirarchyData(m_RootNode, scene->mRootNode);
        ReadMissingBones(animation, model);
    }

    Bone* FindBone(const std::string& name)
    {
        auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
            [&](const Bone& Bone)
            {
                return Bone.m_Name == name;
            }
        );
        if (iter == m_Bones.end()) return nullptr;
        else return &(*iter);
    }


    inline float GetTicksPerSecond() { return m_TicksPerSecond; }

    inline float GetDuration() { return m_Duration; }

    inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

    inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
    {
        return m_BoneInfoMap;
    }


    void ReadMissingBones(const aiAnimation* animation, std::shared_ptr<Model> model);

    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
};

class Animator
{
public:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    std::weak_ptr<Animation> m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;

    bool isLoop = false;
    float animSpeedPer = 1.0f;
    bool isPlay = false;

    Animator(std::shared_ptr<Animation> animation);
    Animator();
    void Update(float dt);
    void Play();
    void Stop();
    void Pause();
    void Reset();

    void SetAnimation(std::shared_ptr<Animation> pAnimation);
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

    std::vector<glm::mat4> GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }
};

Animator::Animator(std::shared_ptr<Animation> animation)
{
    SetAnimation(animation);
    isPlay = true;
    for (int i = m_FinalBoneMatrices.size(); i < BoneMaxCount; i++)
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}
Animator::Animator()
{
    for (int i = m_FinalBoneMatrices.size(); i < BoneMaxCount; i++)
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    isPlay = false;
}

void Animator::Update(float dt)
{
    m_DeltaTime = dt * animSpeedPer;
    if (auto animation = m_CurrentAnimation.lock())
    {
        m_CurrentTime += animation->GetTicksPerSecond() * dt;
        if(isLoop)
            m_CurrentTime = fmod(m_CurrentTime, animation->GetDuration());
        else
        {
            m_CurrentTime = Min(m_CurrentTime, animation->GetDuration());
            isPlay = false;
        }
        
        CalculateBoneTransform(&animation->GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::SetAnimation(std::shared_ptr<Animation> pAnimation)
{
    m_CurrentAnimation = pAnimation;
    m_CurrentTime = 0.0f;
    isPlay = true;
}

void Animator::Play()
{
    if (auto animation = m_CurrentAnimation.lock())
        this->isPlay = true;
    else
        WarringLog() << "애니메이션이 없음\n";
}
void Animator::Pause()
{
    this->isPlay = false;
}
void Animator::Stop()
{
    this->isPlay = false;
    this->m_CurrentTime = 0.0f;
}
void Animator::Reset()
{
    this->m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;
    if (auto animation = m_CurrentAnimation.lock())
    {
        Bone* Bone = animation->FindBone(nodeName);

        if (Bone != nullptr)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = animation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}

void Animation::ReadMissingBones(const aiAnimation* animation, std::shared_ptr<Model> model)
{
    int size = animation->mNumChannels;

    auto& boneInfoMap = model->boneInfoMap;//getting m_BoneInfoMap from Model class
    int& boneCount = model->boneCounter; //getting the m_BoneCounter from Model class
    //reading channels(bones engaged in an animation and their keyframes)
    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        m_Bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
    }
    m_BoneInfoMap = boneInfoMap;
}

void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
{
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (int i = 0; i < src->mNumChildren; i++)
    {
        AssimpNodeData newData;
        ReadHeirarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}


class Mesh
{
public:
    bool hasNormal = false;
    bool hasTangent = false;
    bool hasColors[8] = { false, false, false, false, false, false, false, false };
    bool hasUVs[8] = { false, false, false, false, false, false, false, false };
    bool hasBone = false;

    std::vector<MeshVertex> vertexs;
    std::vector<std::vector<unsigned int>> faceAndindexs;
    std::vector<MeshTexture> textures;

    std::shared_ptr<ModelInfo2> processData();
};




class ShaderCode
{
public:
    const char* path = "";
    char* code = nullptr;
    unsigned int shaderCodeID = UINT32_MAX;
    unsigned int shaderType = GL_VERTEX_SHADER;
    bool isShaderLoad = false;
    bool isShaderCompiled = false;

    void Delete()
    {
        if (shaderCodeID != UINT32_MAX)
            glDeleteShader(shaderCodeID);
        if (code != nullptr)
            delete[] code;
        NormalLog(std::cerr, "Try", 1) << "Shader Code Delete - Memory Clear Path : " << path << "\n";
    }
};

class Shader
{
public:
    static unsigned int currentVAOID;
    static unsigned int currentShaderID;
    static std::vector<const char*> indexAttributeNameList;

    unsigned int shaderID = UINT32_MAX;
    ShaderCode vertexCode;
    ShaderCode fragmentCode;
    bool isCompleted = false;

    std::vector<AttributeSegment> attributeInfos;
    std::shared_ptr<UniformStorage> uniformStorage;

    Shader()
    {
        uniformStorage = std::shared_ptr<UniformStorage>(new UniformStorage());
    }

    void Delete()
    {
        if (this->shaderID != UINT32_MAX)
            glDeleteProgram(this->shaderID);
        attributeInfos.clear();
        isCompleted = false;
    }
    void Bind()
    {
        if (currentShaderID != this->shaderID)
        {
            glUseProgram(this->shaderID);
            currentShaderID = this->shaderID;
        }
    }
    void Init()
    {
        if (isCompleted && this->shaderID != UINT32_MAX)
        {
            attributeInfos.clear();

            NormalLog(std::cerr, "Log") << "Attribute를 읽어서 Infos에 추가." << std::endl;

            int count = 0;
            glGetProgramiv(this->shaderID, GL_ACTIVE_ATTRIBUTES, &count);
            if (count == 0)
            {
                NormalLog(std::cerr, "Info", 1) << "Attribute: 해당 Shader는 아무 Attribute도 없음." << std::endl;
                return;
            }

            GLsizei bufSize = 256;
            GLint size;
            GLsizei length;
            GLenum type;
            char* name = new char[bufSize];
            for (int idx = 0; idx < count; idx++)
            {
                glGetActiveAttrib(this->shaderID, idx, bufSize, &length, &size, &type, name);
                NormalLog(std::cerr, "Info", 1) << "Attribute: name:" << name << ", nameLength:" << length << ", size:" << size << ", type:" << type << "\n";
                int elementType = type;
                int col = 1;
                int row = size;
                GL_TypeToSplitType(&type, &col, &row);
                //NormalLog(std::cerr, "Try") << "Auto Push Attribute Info\n";
                char* name2;
                std::string(name).copy(name2 = new char[length + 1](), length);
                SetAttribute(name2, col, row, type, elementType, GL_FLOAT);
                delete[] name2;
            }

            int uniformCount = 0;
            glGetProgramiv(this->shaderID, GL_ACTIVE_UNIFORMS, &count);
            for (int idx = 0; idx < count; idx++)
            {
                glGetActiveUniform(this->shaderID, idx, bufSize, &length, &size, &type, name);
                NormalLog(std::cerr, "Info", 1) << "Uniform: name:" << name << ", nameLength:" << length << ", size:" << size << ", type:" << type << "\n";

                int elementType = GL_FLOAT;
                int col = 1;
                int row = size;
                char* name2;
                std::string(name).copy(name2 = new char[length + 1](), length);
                SetUniform(name, type);
                delete[] name2;
                //
            }
            uniformStorage->BindShader(this);
        }
    }
    void SetAttribute(const char* name, int col, int row, unsigned int rawType, unsigned int type, unsigned int normalized)
    {
        GLint positionAttribute = glGetAttribLocation(this->shaderID, name);
        if (positionAttribute == -1) {
            WarringLog(std::cerr, "Warring", 2) << "Failed SetAttribute" << " - Shader에 존재하지 않는 Attribute. name : " << name << "\n";
            return;
        }
        int findIndex = -1;
        for (int i = 0; i < attributeInfos.size(); i++)
            if (strcmp(attributeInfos[i].name, name) == 0)
            {
                findIndex = i;
                break;
            }
        if (findIndex == -1)
        {
            NormalLog(std::cerr, "Try", 2) << "New Add SetAttribute" << " - name : " << name << "\n";
            attributeInfos.push_back(AttributeSegment( name, positionAttribute, col, row, rawType, type, normalized ));
        }
        else
        {
            NormalLog(std::cerr, "Try", 2) << "Update SetAttribute" << " - name : " << name << "\n";
            attributeInfos[findIndex] = AttributeSegment( name, positionAttribute, col, row, rawType, type, normalized );
        }
    }
    void SetUniform(const char* name, unsigned int type)
    {
        GLint location = glGetUniformLocation(this->shaderID, name);
        if (location == -1) {
            WarringLog(std::cerr, "Warring", 2) << "Failed SetSetUniform" << " - Shader에 존재하지 않는 Attribute. name : " << name << "\n";
            return;
        }
        NormalLog(std::cerr, "Try", 2) << "Add Uniform" << " - name : " << name << "\n";
        uniformStorage->PushUniform(UniformSegment(name, type));
    }

    static void* GL_TypeToNewArray(unsigned int type, int size)
    {
        switch (type)
        {
        case GL_FLOAT:              return new float[size]();
        case GL_DOUBLE:             return new double[size]();
        case GL_INT:                return new int[size]();
        case GL_BYTE:               return new char[size]();
        case GL_SHORT:              return new short[size]();
        case GL_UNSIGNED_INT:       return new unsigned int[size]();
        case GL_UNSIGNED_BYTE:      return new unsigned char[size]();
        case GL_UNSIGNED_SHORT:     return new unsigned short[size]();
        case GL_BOOL:               return new bool[size]();
        }
        ErrorLog(std::cerr, "Error", 0) << "Array 생성 실패, 존재하지 않는 타입.\n" << std::endl;
        return nullptr;
    }
    static int GL_TypeToSizeOf(unsigned int type)
    {
        switch (type)
        {
        case 0:                     return 0;
        case GL_FLOAT:              return sizeof(float) * 1;
        case GL_FLOAT_VEC2:         return sizeof(float) * 2;
        case GL_FLOAT_VEC3:         return sizeof(float) * 3;
        case GL_FLOAT_VEC4:         return sizeof(float) * 4;
        case GL_FLOAT_MAT2:         return sizeof(float) * 4;
        case GL_FLOAT_MAT3:         return sizeof(float) * 9;
        case GL_FLOAT_MAT4:         return sizeof(float) * 16;
        case GL_FLOAT_MAT2x3:       return sizeof(float) * 6;
        case GL_FLOAT_MAT2x4:       return sizeof(float) * 8;
        case GL_FLOAT_MAT3x2:       return sizeof(float) * 6;
        case GL_FLOAT_MAT3x4:       return sizeof(float) * 12;
        case GL_FLOAT_MAT4x2:       return sizeof(float) * 8;
        case GL_FLOAT_MAT4x3:       return sizeof(float) * 12;

        case GL_DOUBLE:             return sizeof(double) * 1;
        case GL_DOUBLE_VEC2:        return sizeof(double) * 2;
        case GL_DOUBLE_VEC3:        return sizeof(double) * 3;
        case GL_DOUBLE_VEC4:        return sizeof(double) * 4;
        case GL_DOUBLE_MAT2:        return sizeof(double) * 4;
        case GL_DOUBLE_MAT3:        return sizeof(double) * 9;
        case GL_DOUBLE_MAT4:        return sizeof(double) * 16;
        case GL_DOUBLE_MAT2x3:      return sizeof(double) * 6;
        case GL_DOUBLE_MAT2x4:      return sizeof(double) * 8;
        case GL_DOUBLE_MAT3x2:      return sizeof(double) * 6;
        case GL_DOUBLE_MAT3x4:      return sizeof(double) * 12;
        case GL_DOUBLE_MAT4x2:      return sizeof(double) * 8;
        case GL_DOUBLE_MAT4x3:      return sizeof(double) * 12;

        case GL_INT:                return sizeof(int) * 1;
        case GL_INT_VEC2:           return sizeof(int) * 1;
        case GL_INT_VEC3:           return sizeof(int) * 1;
        case GL_INT_VEC4:           return sizeof(int) * 1;

        case GL_UNSIGNED_INT:       return sizeof(unsigned int) * 1;
        case GL_UNSIGNED_INT_VEC2:  return sizeof(unsigned int) * 1;
        case GL_UNSIGNED_INT_VEC3:  return sizeof(unsigned int) * 1;
        case GL_UNSIGNED_INT_VEC4:  return sizeof(unsigned int) * 1;

        }
        WarringLog(std::cerr, "Warring", 0) << "Type의 크기를 찾지 못함.\n";
        return 0;
    }

    static void GL_TypeToSplitType(unsigned int* _type, int* _col, int* _row)
    {
        unsigned int elementType = *_type;
        int col = *_col;
        int row = *_row;

        switch (*_type)
        {
        case GL_FLOAT:              elementType = GL_FLOAT; col = 1; row = 1; break;
        case GL_FLOAT_VEC2:         elementType = GL_FLOAT; col = 2; row = 1; break;
        case GL_FLOAT_VEC3:         elementType = GL_FLOAT; col = 3; row = 1; break;
        case GL_FLOAT_VEC4:         elementType = GL_FLOAT; col = 4; row = 1; break;
        case GL_FLOAT_MAT2:         elementType = GL_FLOAT; col = 2; row = 2; break;
        case GL_FLOAT_MAT3:         elementType = GL_FLOAT; col = 3; row = 3; break;
        case GL_FLOAT_MAT4:         elementType = GL_FLOAT; col = 4; row = 4; break;
        case GL_FLOAT_MAT2x3:       elementType = GL_FLOAT; col = 2; row = 3; break;
        case GL_FLOAT_MAT2x4:       elementType = GL_FLOAT; col = 2; row = 4; break;
        case GL_FLOAT_MAT3x2:       elementType = GL_FLOAT; col = 3; row = 2; break;
        case GL_FLOAT_MAT3x4:       elementType = GL_FLOAT; col = 3; row = 4; break;
        case GL_FLOAT_MAT4x2:       elementType = GL_FLOAT; col = 4; row = 2; break;
        case GL_FLOAT_MAT4x3:       elementType = GL_FLOAT; col = 4; row = 3; break;

        case GL_DOUBLE:             elementType = GL_DOUBLE; col = 1; row = 1; break;
        case GL_DOUBLE_VEC2:        elementType = GL_DOUBLE; col = 2; row = 1; break;
        case GL_DOUBLE_VEC3:        elementType = GL_DOUBLE; col = 3; row = 1; break;
        case GL_DOUBLE_VEC4:        elementType = GL_DOUBLE; col = 4; row = 1; break;
        case GL_DOUBLE_MAT2:        elementType = GL_DOUBLE; col = 2; row = 2; break;
        case GL_DOUBLE_MAT3:        elementType = GL_DOUBLE; col = 3; row = 3; break;
        case GL_DOUBLE_MAT4:        elementType = GL_DOUBLE; col = 4; row = 4; break;
        case GL_DOUBLE_MAT2x3:      elementType = GL_DOUBLE; col = 2; row = 3; break;
        case GL_DOUBLE_MAT2x4:      elementType = GL_DOUBLE; col = 2; row = 4; break;
        case GL_DOUBLE_MAT3x2:      elementType = GL_DOUBLE; col = 3; row = 2; break;
        case GL_DOUBLE_MAT3x4:      elementType = GL_DOUBLE; col = 3; row = 4; break;
        case GL_DOUBLE_MAT4x2:      elementType = GL_DOUBLE; col = 4; row = 2; break;
        case GL_DOUBLE_MAT4x3:      elementType = GL_DOUBLE; col = 4; row = 3; break;

        case GL_INT:                elementType = GL_INT; col = 1; row = 1; break;
        case GL_INT_VEC2:           elementType = GL_INT; col = 2; row = 1; break;
        case GL_INT_VEC3:           elementType = GL_INT; col = 3; row = 1; break;
        case GL_INT_VEC4:           elementType = GL_INT; col = 4; row = 1; break;

        case GL_UNSIGNED_INT:       elementType = GL_UNSIGNED_INT; col = 1; row = 1; break;
        case GL_UNSIGNED_INT_VEC2:  elementType = GL_UNSIGNED_INT; col = 2; row = 1; break;
        case GL_UNSIGNED_INT_VEC3:  elementType = GL_UNSIGNED_INT; col = 3; row = 1; break;
        case GL_UNSIGNED_INT_VEC4:  elementType = GL_UNSIGNED_INT; col = 4; row = 1; break;
        }

        *_type = elementType;
        *_col = col;
        *_row = row;
    }
};
unsigned int Shader::currentShaderID = UINT_MAX;
unsigned int Shader::currentVAOID = 0;
std::vector<const char*> Shader::indexAttributeNameList = std::vector<const char*>({ "vertexIndex", "vertexColorIndex", "normalIndex", "uv0Index" });
Shader errorShader;



class VBOInfo
{
public:
    VBOInfo() = default;
    VBOInfo(void* dataBufferAddress,
        int dataBufferSize,
        unsigned int bufferType,
        unsigned int jumpSize,
        unsigned int offset)
    {
        this->dataBufferAddress = dataBufferAddress;
        this->dataBufferSize = dataBufferSize;
        this->bufferType = bufferType;
        this->jumpSize = jumpSize;
        this->offset = offset;
    }

    void* dataBufferAddress = nullptr;
    int dataBufferSize = 0;
    unsigned int bufferType = GL_ARRAY_BUFFER;
    unsigned int jumpSize = 0;
    unsigned int offset = 0;
};



int UniformSegment::BindShader(Shader* shader)
{
    int location = glGetUniformLocation(shader->shaderID, name);
    if (location != -1)
    {
        this->shaderID = shader->shaderID;
        this->location = location;
    }
    return location;
}
UniformSegment::UniformSegment(const char* name, unsigned int type)
{
    int nameLength = strlen(name);
    strncpy((this->name = new char[nameLength + 1]()), name, nameLength);
    this->type = type;
}

UniformSegment::UniformSegment(const UniformSegment& origin)
{
    int nameLength = strlen(origin.name);
    strncpy((this->name = new char[nameLength + 1]()), origin.name, nameLength);
    this->type = origin.type;
    this->location = origin.location;

    this->data_vector = origin.data_vector;
    this->data_vector_int = origin.data_vector_int;
    this->data_matrix2 = origin.data_matrix2;
    this->data_matrix3 = origin.data_matrix3;
    this->data_matrix4 = origin.data_matrix4;
}
UniformSegment& UniformSegment::operator =(const UniformSegment& origin)
{
    int nameLength = strlen(origin.name);
    strncpy((this->name = new char[nameLength + 1]()), origin.name, nameLength);
    this->type = origin.type;
    this->location = origin.location;

    this->data_vector = origin.data_vector;
    this->data_vector_int = origin.data_vector_int;
    this->data_matrix2 = origin.data_matrix2;
    this->data_matrix3 = origin.data_matrix3;
    this->data_matrix4 = origin.data_matrix4;

    return *this;
}

UniformSegment& UniformSegment::operator =(UniformSegment&& origin)
{
    this->name = origin.name;
    this->type = origin.type;
    this->location = origin.location;

    this->data_vector = origin.data_vector;
    this->data_vector_int = origin.data_vector_int;
    this->data_matrix2 = origin.data_matrix2;
    this->data_matrix3 = origin.data_matrix3;
    this->data_matrix4 = origin.data_matrix4;

    origin.name = nullptr;
    return *this;
}
UniformSegment::UniformSegment(UniformSegment&& origin)
{
    this->name = origin.name;
    this->type = origin.type;
    this->location = origin.location;

    this->data_vector = origin.data_vector;
    this->data_vector_int = origin.data_vector_int;
    this->data_matrix2 = origin.data_matrix2;
    this->data_matrix3 = origin.data_matrix3;
    this->data_matrix4 = origin.data_matrix4;

    origin.name = nullptr;
}
void UniformSegment::UpdateUniformSegment(UniformSegment& origin)
{
    this->data_vector = origin.data_vector;
    this->data_vector_int = origin.data_vector_int;
    this->data_matrix2 = origin.data_matrix2;
    this->data_matrix3 = origin.data_matrix3;
    this->data_matrix4 = origin.data_matrix4;
}

void UniformSegment::Bind()
{
    if(location == -1)
        location = glGetUniformLocation(Shader::currentShaderID, name);

    if (location != -1)
    {
        switch (type)
        {
        case GL_FLOAT:              glUniform1f(location, data_vector.x); break;
        case GL_FLOAT_VEC2:         glUniform2f(location, data_vector.x, data_vector.y); break;
        case GL_FLOAT_VEC3:         glUniform3f(location, data_vector.x, data_vector.y, data_vector.z); break;
        case GL_FLOAT_VEC4:         glUniform4f(location, data_vector.x, data_vector.y, data_vector.z, data_vector.w); break;
        case GL_FLOAT_MAT2:         glUniformMatrix2fv(location, 1, normalized, glm::value_ptr(data_matrix2)); break;
        case GL_FLOAT_MAT3:         glUniformMatrix3fv(location, 1, normalized, glm::value_ptr(data_matrix3)); break;
        case GL_FLOAT_MAT4:         glUniformMatrix4fv(location, 1, normalized, glm::value_ptr(data_matrix4)); break;

        case GL_DOUBLE:             glUniform1d(location, data_vector.x); break;
        case GL_DOUBLE_VEC2:        glUniform2d(location, data_vector.x, data_vector.y); break;
        case GL_DOUBLE_VEC3:        glUniform3d(location, data_vector.x, data_vector.y, data_vector.z); break;
        case GL_DOUBLE_VEC4:        glUniform4d(location, data_vector.x, data_vector.y, data_vector.z, data_vector.w); break;
        case GL_DOUBLE_MAT2:        glUniformMatrix2dv(location, 1, normalized, (double*)glm::value_ptr(glm::highp_mat2(data_matrix2))); break;
        case GL_DOUBLE_MAT3:        glUniformMatrix3dv(location, 1, normalized, (double*)glm::value_ptr(glm::highp_mat3(data_matrix3))); break;
        case GL_DOUBLE_MAT4:        glUniformMatrix4dv(location, 1, normalized, (double*)glm::value_ptr(glm::highp_mat4(data_matrix4))); break;
            
        case GL_INT:                glUniform1i(location, data_vector_int.x); break;
        case GL_INT_VEC2:           glUniform2i(location, data_vector_int.x, data_vector_int.y); break;
        case GL_INT_VEC3:           glUniform3i(location, data_vector_int.x, data_vector_int.y, data_vector_int.z); break;
        case GL_INT_VEC4:           glUniform4i(location, data_vector_int.x, data_vector_int.y, data_vector_int.z, data_vector_int.w); break;

        case GL_UNSIGNED_INT:       glUniform1i(location, data_vector_int.x); break;
        case GL_UNSIGNED_INT_VEC2:  glUniform2i(location, (unsigned int)data_vector_int.x, (unsigned int)data_vector_int.y); break;
        case GL_UNSIGNED_INT_VEC3:  glUniform3i(location, (unsigned int)data_vector_int.x, (unsigned int)data_vector_int.y, (unsigned int)data_vector_int.z); break;
        case GL_UNSIGNED_INT_VEC4:  glUniform4i(location, (unsigned int)data_vector_int.x, (unsigned int)data_vector_int.y, (unsigned int)data_vector_int.z, (unsigned int)data_vector_int.w); break;
        
        case GL_TEXTURE_1D:         glActiveTexture(GL_TEXTURE0 + data_vector_int.x); glBindTexture(GL_TEXTURE_2D, data_vector_int.x); glUniform1i(location, data_vector_int.x); break;
        case GL_TEXTURE_1D_ARRAY:   glActiveTexture(GL_TEXTURE0 + data_vector_int.x); glBindTexture(GL_TEXTURE_2D, data_vector_int.x); glUniform1i(location, data_vector_int.x); break;
        
        case GL_TEXTURE_2D:         glActiveTexture(GL_TEXTURE0 + data_vector_int.x); glBindTexture(GL_TEXTURE_2D, data_vector_int.x); glUniform1i(location, data_vector_int.x); break;
        case GL_TEXTURE_2D_ARRAY:   glActiveTexture(GL_TEXTURE0 + data_vector_int.x); glBindTexture(GL_TEXTURE_2D, data_vector_int.x); glUniform1i(location, data_vector_int.x); break;

        case GL_TEXTURE_3D:         glActiveTexture(GL_TEXTURE0 + data_vector_int.x); glBindTexture(GL_TEXTURE_2D, data_vector_int.x); glUniform1i(location, data_vector_int.x); break;

        case GL_SAMPLER_1D:         glActiveTexture(GL_TEXTURE0 + data_vector_int.x); glBindTexture(GL_TEXTURE_2D, data_vector_int.x); glUniform1i(location, data_vector_int.x); break;
        case GL_SAMPLER_2D:         glActiveTexture(GL_TEXTURE0 + data_vector_int.x); glBindTexture(GL_TEXTURE_2D, data_vector_int.x); glUniform1i(location, data_vector_int.x); break;
        case GL_SAMPLER_3D:         glActiveTexture(GL_TEXTURE0 + data_vector_int.x); glBindTexture(GL_TEXTURE_2D, data_vector_int.x); glUniform1i(location, data_vector_int.x); break;
        case GL_SAMPLER_CUBE:       glActiveTexture(GL_TEXTURE0 + data_vector_int.x); glBindTexture(GL_TEXTURE_2D, data_vector_int.x); glUniform1i(location, data_vector_int.x); break;
        case GL_SAMPLER_1D_SHADOW:  glActiveTexture(GL_TEXTURE0 + data_vector_int.x); glBindTexture(GL_TEXTURE_2D, data_vector_int.x); glUniform1i(location, data_vector_int.x); break;
        case GL_SAMPLER_2D_SHADOW:  glActiveTexture(GL_TEXTURE0 + data_vector_int.x); glBindTexture(GL_TEXTURE_2D, data_vector_int.x); glUniform1i(location, data_vector_int.x); break;

        //case GL_FLOAT_MAT2x3:       elementType = GL_FLOAT; col = 2; row = 3; break;
        //case GL_FLOAT_MAT2x4:       elementType = GL_FLOAT; col = 2; row = 4; break;
        //case GL_FLOAT_MAT3x2:       elementType = GL_FLOAT; col = 3; row = 2; break;
        //case GL_FLOAT_MAT3x4:       elementType = GL_FLOAT; col = 3; row = 4; break;
        //case GL_FLOAT_MAT4x2:       elementType = GL_FLOAT; col = 4; row = 2; break;
        //case GL_FLOAT_MAT4x3:       elementType = GL_FLOAT; col = 4; row = 3; break;

        //case GL_DOUBLE_MAT2x3:      elementType = GL_DOUBLE; col = 2; row = 3; break;
        //case GL_DOUBLE_MAT2x4:      elementType = GL_DOUBLE; col = 2; row = 4; break;
        //case GL_DOUBLE_MAT3x2:      elementType = GL_DOUBLE; col = 3; row = 2; break;
        //case GL_DOUBLE_MAT3x4:      elementType = GL_DOUBLE; col = 3; row = 4; break;
        //case GL_DOUBLE_MAT4x2:      elementType = GL_DOUBLE; col = 4; row = 2; break;
        //case GL_DOUBLE_MAT4x3:      elementType = GL_DOUBLE; col = 4; row = 3; break;
        }


        //glUniformMatrix4fv(location, 1, normalized, glm::value_ptr(modelMatrix));
        //glUniform4f(location, modelcolor.x, modelcolor.y, modelcolor.z, modelcolor.w);
    }
}
UniformSegment::~UniformSegment()
{
    if (this->name != nullptr)
    {
        delete[](this->name);
        this->name = nullptr;
    }
}



void UniformStorage::BindShader(Shader* shader)
{
    for (int i = 0; i < uniformList.size(); i++)
    {
        int location = glGetUniformLocation(shader->shaderID, uniformList[i].name);
        if (uniformList[i].BindShader(shader) == -1)
        {
            uniformList.erase(uniformList.begin() + i);
            i--;
            WarringLog(std::cerr, "Warring", 1) << "사용하지 않는 Uniform 목록에서 제거됨 - name : " << uniformList[i].name << "\n";
            continue;
        }
    }
}



AttributeSegment::AttributeSegment(const char* name, int segmentCount)
{
    int nameLength = strlen(name);
    strncpy((this->name = new char[nameLength + 1]()), name, nameLength);
    this->segmentCount = segmentCount;

    this->rawBuffer;
    this->offsetCount = -1;
    this->blockCount = -1;
    this->type = GL_FLOAT;
    this->rawType = this->type;
    this->typeSize = -1;
}
AttributeSegment::AttributeSegment(const char* name, int location, int col, int row, unsigned int rawType, unsigned int type, unsigned int normalized)
{
    int nameLength = strlen(name);
    strncpy((this->name = new char[nameLength + 1]()), name, nameLength);

    this->rawBuffer;
    this->offsetCount = -1;
    this->blockCount = -1;
    this->typeSize = -1;

    this->location = location;
    this->col = col;
    this->row = row;
    this->rawType = rawType;
    this->type = type;
    this->normalized = normalized;
}

AttributeSegment::AttributeSegment(const AttributeSegment& origin)
{
    int nameLength = strlen(origin.name);
    strncpy((this->name = new char[nameLength + 1]()), origin.name, nameLength);
    this->segmentCount = origin.segmentCount;

    this->rawBuffer = origin.rawBuffer;
    this->offsetCount = origin.offsetCount;
    this->blockCount = origin.blockCount;
    this->type = origin.type;
    this->rawType = origin.rawType;
    this->typeSize = origin.typeSize;
    this->location = origin.location;
    this->col = origin.col;
    this->row = origin.row;
    this->normalized = origin.normalized;
}
AttributeSegment::AttributeSegment(AttributeSegment&& other)
{
    this->name = other.name;
    this->segmentCount = other.segmentCount;

    this->rawBuffer = other.rawBuffer;
    this->offsetCount = other.offsetCount;
    this->blockCount = other.blockCount;
    this->type = other.type;
    this->rawType = other.rawType;
    this->typeSize = other.typeSize;
    this->location = other.location;
    this->col = other.col;
    this->row = other.row;
    this->normalized = other.normalized;
    other.name = nullptr;
}
AttributeSegment& AttributeSegment::operator =(const AttributeSegment& other)
{
    int nameLength = strlen(other.name);
    strncpy((this->name = new char[nameLength + 1]()), other.name, nameLength);
    this->segmentCount = other.segmentCount;

    this->rawBuffer = other.rawBuffer;
    this->offsetCount = other.offsetCount;
    this->blockCount = other.blockCount;
    this->type = other.type;
    this->rawType = other.rawType;
    this->typeSize = other.typeSize;
    this->location = other.location;
    this->col = other.col;
    this->row = other.row;
    this->normalized = other.normalized;
    return *this;
}
AttributeSegment& AttributeSegment::operator =(AttributeSegment&& other)
{
    this->name = other.name;
    this->segmentCount = other.segmentCount;

    this->rawBuffer = other.rawBuffer;
    this->offsetCount = other.offsetCount;
    this->blockCount = other.blockCount;
    this->type = other.type;
    this->rawType = other.rawType;
    this->typeSize = other.typeSize;
    this->location = other.location;
    this->col = other.col;
    this->row = other.row;
    this->normalized = other.normalized;
    other.name = nullptr;
    return *this;
}

AttributeSegment& AttributeSegment::ConvertRealSegment(std::shared_ptr<void> rawBuffer, unsigned int type, int offsetCount, int blockCount)
{
    this->rawBuffer = rawBuffer;
    this->rawType = this->type;
    this->type = (int)type;
    this->typeSize = Shader::GL_TypeToSizeOf(type);
    this->offsetCount = offsetCount;
    this->blockCount = blockCount;
    return *this;
}
void AttributeSegment::Destroy()
{
    if (this->name != nullptr)
    {
        delete[](this->name);
        this->name = nullptr;
    }
    segmentCount = -1;
}
AttributeSegment::~AttributeSegment()
{
    this->Destroy();
}


class ModelBufferData
{
public:
    int GUID = -1;
    std::shared_ptr<void> rawBuffer = nullptr;
    unsigned int glType = GL_FLOAT;
    int size = -1;
    int typeSize = -1;
    std::vector<AttributeSegment> attributeInfos;

    void NewGUID()
    {
        this->GUID = RandomInt();
    }

    ModelBufferData(std::shared_ptr<void> address, int size, unsigned int type, std::vector<AttributeSegment> segments)
    {
        int totalCount = 0;
        int totalSize = 0;
        for (int i = 0; i < segments.size(); i++)
            totalCount += segments[i].segmentCount;
        totalSize = totalCount;

        NewGUID();
        this->rawBuffer = address;
        this->glType = type;
        this->size = size;
        this->typeSize = Shader::GL_TypeToSizeOf(this->glType);

        if (attributeInfos.size() != 0)
            attributeInfos.clear();
        attributeInfos = segments;
    }
    ModelBufferData(void* address, int size, unsigned int type, std::vector<AttributeSegment>& segments) : ModelBufferData(std::shared_ptr<void>(address), size, type, segments)
    {
    }
    ModelBufferData(void* address, int size, unsigned int type, std::vector<AttributeSegment>&& segments) : ModelBufferData(std::shared_ptr<void>(address), size, type, segments)
    {
    }
    int GetBlockCount()
    {
        int result = 0;
        for (int i = 0; i < attributeInfos.size(); i++)
            result += attributeInfos[i].segmentCount;
        return result;
    }
    bool BufferResize(std::shared_ptr<void*> address, int size)
    {
        this->rawBuffer = address;
        this->size = size;
    }

    int FindIndexAttributeAsName(char* name) // 찾기 실패시 -1
    {
        for (int i = 0; i < attributeInfos.size(); i++)
            if (strcmp(attributeInfos[i].name, name) == 0)
                return i;
        return -1;
    }
    bool FindAbleAttributeAsName(char* name)
    {
        for (int i = 0; i < attributeInfos.size(); i++)
        {
            if (strcmp(attributeInfos[i].name, name) == 0)
                return true;
        }
        return false;
    }
    AttributeSegment FindAttributeAsName(char* name)
    {
        for (int i = 0; i < attributeInfos.size(); i++)
        {
            if (strcmp(attributeInfos[i].name, name) == 0)
                return attributeInfos[i];
        }
        return { "none", 0 };
    }
    ModelBufferData(const ModelBufferData& origin)
    {
        this->attributeInfos = origin.attributeInfos;
        this->glType = origin.glType;
        this->size = origin.size;
        this->typeSize = origin.typeSize;
        this->rawBuffer = origin.rawBuffer;
        this->GUID = origin.GUID;
    }
    ModelBufferData& operator =(const ModelBufferData& origin)
    {
        this->attributeInfos = origin.attributeInfos;
        this->glType = origin.glType;
        this->size = origin.size;
        this->typeSize = origin.typeSize;
        this->rawBuffer = origin.rawBuffer;
        this->GUID = origin.GUID;
        return *this;
    }
    ~ModelBufferData()
    {
        attributeInfos.clear();
    }
    void DeepCopy()
    {

    }
    void Destroy()
    {
        rawBuffer = nullptr;
        for (int i = 0; i < attributeInfos.size(); i++)
            attributeInfos[i].Destroy();
        attributeInfos.clear();
    }
};
class ModelInfo2
{
public:
    std::vector<ModelBufferData> datas;

    ModelInfo2()
    {

    }
    ModelInfo2(std::vector<ModelBufferData> datas)
    {
        this->datas = datas;
    }
    static std::vector<AttributeSegment> GetFilteringAttributes(std::vector<ModelBufferData> datas)
    {
        std::vector<AttributeSegment> resultAttributes;
        for (int j = 0; j < datas.size(); j++)
        {
            ModelBufferData& nowBufferData = datas[j];
            std::vector<AttributeSegment>& attributes = nowBufferData.attributeInfos;

            int offsetSumCalculate = 0;
            int totalCountSumCalculate = 0;
            for (int i = 0; i < attributes.size(); i++)
                totalCountSumCalculate += attributes[i].segmentCount;
            for (int i = 0; i < attributes.size(); i++)
            {
                auto nowAttribute = attributes[i];
                resultAttributes.push_back(nowAttribute.ConvertRealSegment(nowBufferData.rawBuffer, nowBufferData.glType, offsetSumCalculate, totalCountSumCalculate));
                offsetSumCalculate += nowAttribute.segmentCount;
            }
        }
        for (int i = resultAttributes.size() - 1; i >= 0; i--)
        {
            AttributeSegment& nowAttribute = resultAttributes[i];
            int j = 0;
            for (j = i + 1; j < resultAttributes.size(); j++)
            {
                AttributeSegment& checkAttributes = resultAttributes[j];
                if (strcmp(nowAttribute.name, checkAttributes.name) == 0)
                    break;
            }
            if (j != resultAttributes.size()) // 이미 존재하는 녀석.
                resultAttributes.erase(resultAttributes.begin() + i);
        }
        return resultAttributes;
    }
    static std::vector<ModelBufferData> GetFilteringBufferDatas(std::vector<ModelBufferData> datas)
    {
        std::vector<ModelBufferData> resultBuferDatas;
        //RealAttribute Convert
        for (int j = 0; j < datas.size(); j++)
        {
            ModelBufferData nowBufferData = datas[j];
            std::vector<AttributeSegment>& attributes = nowBufferData.attributeInfos;
            std::vector<AttributeSegment> resultAttributes;

            int offsetSumCalculate = 0;
            int totalCountSumCalculate = 0;
            for (int i = 0; i < attributes.size(); i++)
                totalCountSumCalculate += attributes[i].segmentCount;
            for (int i = 0; i < attributes.size(); i++)
            {
                auto nowAttribute = attributes[i];
                resultAttributes.push_back(nowAttribute.ConvertRealSegment(nowBufferData.rawBuffer, nowBufferData.glType, offsetSumCalculate, totalCountSumCalculate));
                offsetSumCalculate += nowAttribute.segmentCount;
            }
            nowBufferData.attributeInfos = resultAttributes;
            resultBuferDatas.push_back(nowBufferData);
            resultAttributes.clear();
        }
        // Remove duplicated
        std::vector<AttributeSegment> checkSumAttributes;
        //for (int i = 0; i < resultBuferDatas.size(); i++) //정방향
        for (int i = resultBuferDatas.size() - 1; i >= 0; i--) //역방향
        {
            for (int j = 0; j < resultBuferDatas[i].attributeInfos.size(); j++)
            {
                int duplicatedIndex = -1;
                for (int k = 0; k < checkSumAttributes.size(); k++)
                    if (strcmp(checkSumAttributes[k].name, resultBuferDatas[i].attributeInfos[j].name) == 0)
                        duplicatedIndex = k;
                if (duplicatedIndex != -1)
                {
                    resultBuferDatas[i].attributeInfos.erase(resultBuferDatas[i].attributeInfos.begin() + j);
                    j--;
                }
                else
                {
                    checkSumAttributes.push_back(resultBuferDatas[i].attributeInfos[j]);
                }
            }
            if (resultBuferDatas[i].attributeInfos.size() == 0)
                resultBuferDatas.erase(resultBuferDatas.begin() + i);
            /* 정방향
            if (resultBuferDatas[i].attributeInfos.size() == 0){
                resultBuferDatas.erase(resultBuferDatas.begin() + i); i--}
            */
        }
        checkSumAttributes.clear();

        return resultBuferDatas;
    }
    static std::vector<ModelBufferData> GetFilteringShaderBufferDatas(std::vector<ModelBufferData> datas, Shader* shader)
    {
        std::vector<ModelBufferData> resultBuferDatas = std::vector<ModelBufferData>(datas);
        for (int i = 0; i < resultBuferDatas.size(); i++)
        {
            for (int j = 0; j < resultBuferDatas[i].attributeInfos.size(); j++)
            {
                int shaderAttributeIndex = -1;
                for (int k = 0; k < shader->attributeInfos.size(); k++)
                    if (strcmp(resultBuferDatas[i].attributeInfos[j].name, shader->attributeInfos[k].name) == 0)
                    {
                        shaderAttributeIndex = k;
                        resultBuferDatas[i].attributeInfos[j].location = shader->attributeInfos[k].location;
                        resultBuferDatas[i].attributeInfos[j].col = shader->attributeInfos[k].col;
                        resultBuferDatas[i].attributeInfos[j].row = shader->attributeInfos[k].row;
                        resultBuferDatas[i].attributeInfos[j].normalized = shader->attributeInfos[k].normalized;
                    }
                if (shaderAttributeIndex == -1)
                {
                    resultBuferDatas[i].attributeInfos.erase(resultBuferDatas[i].attributeInfos.begin() + j);
                    j--;
                }
            }
            if (resultBuferDatas[i].attributeInfos.size() == 0)
            {
                resultBuferDatas.erase(resultBuferDatas.begin() + i);
                i--;
            }
        }
        return resultBuferDatas;
    }
    static std::vector<ModelBufferData> GetFilteringIndexBuffers(std::vector<ModelBufferData> datas)
    {
        std::vector<ModelBufferData> resultBuffer;
        for (int i = 0; i < datas.size(); i++)
        {
            for (int j = 0; j < datas[i].attributeInfos.size(); j++)
            {
                for (int k = 0; k < Shader::indexAttributeNameList.size(); k++)
                    if (strcmp(datas[i].attributeInfos[j].name, Shader::indexAttributeNameList[k]) == 0)
                        resultBuffer.push_back(datas[i]);
            }
        }
        return resultBuffer;
    }
    static ModelBufferData GetConvertPolyToWireIndexBuffer(const ModelBufferData& indexBuffer) // 메모리 누수 주의
    {
        ModelBufferData resultbuffer = indexBuffer;
        resultbuffer.size = resultbuffer.size * 2;
        resultbuffer.rawBuffer = std::shared_ptr<void>(Shader::GL_TypeToNewArray(resultbuffer.glType, resultbuffer.size));
        int typeSize = Shader::GL_TypeToSizeOf(resultbuffer.glType);

        auto youngIndexbuffer = ((char*)resultbuffer.rawBuffer.get());
        auto oldIndexBuffer = ((char*)indexBuffer.rawBuffer.get());
        for (int i = 0; i < indexBuffer.size; i += 3)
        {
            memcpy(&(youngIndexbuffer[(i * 2 + 0) * typeSize]), &(oldIndexBuffer[(i + 0) * typeSize]), typeSize);
            memcpy(&(youngIndexbuffer[(i * 2 + 1) * typeSize]), &(oldIndexBuffer[(i + 1) * typeSize]), typeSize);
            memcpy(&(youngIndexbuffer[(i * 2 + 2) * typeSize]), &(oldIndexBuffer[(i + 1) * typeSize]), typeSize);
            memcpy(&(youngIndexbuffer[(i * 2 + 3) * typeSize]), &(oldIndexBuffer[(i + 2) * typeSize]), typeSize);
            memcpy(&(youngIndexbuffer[(i * 2 + 4) * typeSize]), &(oldIndexBuffer[(i + 2) * typeSize]), typeSize);
            memcpy(&(youngIndexbuffer[(i * 2 + 5) * typeSize]), &(oldIndexBuffer[(i + 0) * typeSize]), typeSize);
        }

        return GetFilteringBufferDatas({ resultbuffer })[0];
    }
    static ModelBufferData GetConvertWireToPolyIndexBuffer(const ModelBufferData& indexBuffer) // 메모리 누수 주의
    {
        ModelBufferData resultbuffer = indexBuffer;
        resultbuffer.size = resultbuffer.size / 2;
        resultbuffer.rawBuffer = std::shared_ptr<void>(Shader::GL_TypeToNewArray(resultbuffer.glType, resultbuffer.size));
        int typeSize = Shader::GL_TypeToSizeOf(resultbuffer.glType);

        auto youngIndexbuffer = ((char*)resultbuffer.rawBuffer.get());
        auto oldIndexBuffer = ((char*)indexBuffer.rawBuffer.get());
        for (int i = 0; i < resultbuffer.size; i += 3)
        {
            memcpy(&(youngIndexbuffer[(i + 0) * typeSize]), &(oldIndexBuffer[(i * 2 + 0) * typeSize]), typeSize);
            memcpy(&(youngIndexbuffer[(i + 1) * typeSize]), &(oldIndexBuffer[(i * 2 + 2) * typeSize]), typeSize);
            memcpy(&(youngIndexbuffer[(i + 2) * typeSize]), &(oldIndexBuffer[(i * 2 + 4) * typeSize]), typeSize);
        }

        return GetFilteringBufferDatas({ resultbuffer })[0];
    }
    static std::vector<ModelBufferData> GetConvertIndexToArrayDataBuffers(ModelBufferData indexBuffer, std::vector<ModelBufferData> dataBuffers)  // 메모리 누수 주의
    {
        std::vector<ModelBufferData> newDataBuffers;
        for (int i = 0; i < dataBuffers.size(); i++)
        {
            newDataBuffers.push_back(dataBuffers[i]);
            auto dataBufferTypeSize = dataBuffers[i].typeSize;
            auto dataBufferBlockSize = dataBuffers[i].GetBlockCount();
            auto byteBlockSize = dataBufferTypeSize * dataBufferBlockSize;
            auto newArrayBuffer = std::shared_ptr<void>(Shader::GL_TypeToNewArray(dataBuffers[i].glType, indexBuffer.size * dataBufferBlockSize));
            auto newRawArrayBuffer = ((char*)newArrayBuffer.get());
            auto oldRawArrayBuffer = ((char*)dataBuffers[i].rawBuffer.get());
            auto rawIndexBuffer = ((char*)indexBuffer.rawBuffer.get());
            int index = 0;
            for (int j = 0; j < indexBuffer.size; j++)
            {
                int indexBufferData = 0;
                for (int k = 0; k < indexBuffer.typeSize; k++)
                    indexBufferData = indexBufferData | (rawIndexBuffer[j * indexBuffer.typeSize + k]) << (8 * k);
                memcpy(&(newRawArrayBuffer[j * byteBlockSize]), &(oldRawArrayBuffer[indexBufferData * byteBlockSize]), byteBlockSize);
            }
            newDataBuffers[i].rawBuffer = newArrayBuffer;
            newDataBuffers[i].size = indexBuffer.size * dataBufferBlockSize;
        }

        return newDataBuffers;
    }
    //위에꺼랑 페어 Index 함수.
    static ModelBufferData GetConvertSequenceIndexBuffers(ModelBufferData indexBuffer)  // 메모리 누수 주의
    {
        ModelBufferData newIndexBuffer = indexBuffer;
        newIndexBuffer.rawBuffer = std::shared_ptr<void>(Shader::GL_TypeToNewArray(indexBuffer.glType, indexBuffer.size));
        auto rawIndexBuffer = ((char*)newIndexBuffer.rawBuffer.get());
        auto size = newIndexBuffer.typeSize;
        for (int i = 0; i < indexBuffer.size; i++)
            memcpy(&rawIndexBuffer[i * size], &i, size);
        return newIndexBuffer;
    }
};

class RenderData
{
public:
    unsigned int VAO = 0;
    unsigned int EBO = 0;

    std::vector<ModelBufferData> modelBufferList;
    std::vector<ModelBufferData> renderBufferList;
    ModelBufferData indexBuffer = { nullptr, 0, 0, {} };
    std::vector<std::pair<int, unsigned int>> table_BufferID_To_VBO;
    
    std::shared_ptr<UniformStorage> uniformStorage;
    Material* material = nullptr;
    ModelInfo2* modelData = nullptr;

    bool activeIndexRender = false;

    float zSorting = 0;

    RenderData(Material* material, ModelInfo2* modelData)
    {
        uniformStorage = std::shared_ptr<UniformStorage>(new UniformStorage());
        Blend(material, modelData);
    }

    void Blend(Material* material, ModelInfo2* modelData)
    {
        this->BindVAO();
        this->material = material;
        this->modelData = modelData;
        this->modelBufferList = ModelInfo2::GetFilteringBufferDatas(this->modelData->datas);
        this->renderBufferList = ModelInfo2::GetFilteringShaderBufferDatas(this->modelBufferList, this->material->shader);
        auto indexBufferList = ModelInfo2::GetFilteringIndexBuffers(this->modelBufferList);

        if (indexBufferList.size() != 0)
        {
            for (int j = 0; j < indexBufferList.size(); j++)
            {
                for (int i = 0; i < Shader::indexAttributeNameList.size(); i++)
                    if (indexBufferList[j].FindAbleAttributeAsName((char*)Shader::indexAttributeNameList[i])) // 처음거 찾으면 인듯?
                    {
                        this->activeIndexRender = true;
                        this->indexBuffer = indexBufferList[j];
                        break;
                    }
                if (activeIndexRender)
                    break;
            }
        }

        //새롭게 갱신.
        UpdateAll();
        AttributesVAOConnect();
        //this->renderBufferList
    }

    void InitVAO()
    {
        if (VAO == 0)
            glGenVertexArrays(1, &VAO);
    }
    void BindVAO()
    {
        this->InitVAO();
        if (Shader::currentVAOID != VAO)
        {
            Shader::currentVAOID = VAO;
            glBindVertexArray(VAO);
        }
    }

    void InitEBO()
    {
        if (EBO == 0)
            glGenBuffers(1, &EBO);
    }
    void BindEBO()
    {
        this->BindVAO();
        this->InitEBO();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    }
    void SetEBO(ModelBufferData indexAttribute)
    {
        this->BindEBO();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexAttribute.typeSize * indexAttribute.size, indexAttribute.rawBuffer.get(), GL_STATIC_DRAW);
    }

    unsigned int FindVBO(ModelBufferData dataBuffers)
    {
        unsigned int VBO = 0;
        for (int i = 0; i < table_BufferID_To_VBO.size(); i++)
        {
            auto nowPair = table_BufferID_To_VBO[i];
            if (nowPair.first == dataBuffers.GUID)
                VBO = nowPair.second;
        }
        return VBO;
    }
    unsigned int InitVBO(ModelBufferData dataBuffer)
    {
        unsigned int VBO = 0;
        for (int i = 0; i < table_BufferID_To_VBO.size(); i++)
        {
            auto nowPair = table_BufferID_To_VBO[i];
            if (nowPair.first == dataBuffer.GUID)
                VBO = nowPair.second;
        }
        if (VBO == 0)
        {
            glGenBuffers(1, &VBO);
            table_BufferID_To_VBO.push_back(std::make_pair(dataBuffer.GUID, VBO));
        }
        return VBO;
    }
    unsigned int BindVBO(ModelBufferData dataBuffer)
    {
        unsigned int VBO = InitVBO(dataBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        return VBO;
    }
    unsigned int BindVBO(unsigned int VBO)
    {
        if (VBO != 0)
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
        return VBO;
    }
    void SetVBO(ModelBufferData dataBuffer)
    {
        BindVAO();
        unsigned int VBO = InitVBO(dataBuffer);
        if (VBO != 0)
        {
            BindVBO(VBO);
            glBufferData(GL_ARRAY_BUFFER, dataBuffer.size * dataBuffer.typeSize, dataBuffer.rawBuffer.get(), GL_STATIC_DRAW);
        }
        //for(int i=0;i< dataBuffers)
    }

    void Bind()
    {

    }

    void AttributesVAOConnect()
    {
        BindVAO();
        for (int i = 0; i < this->renderBufferList.size(); i++)
        {
            BindVBO(FindVBO(this->renderBufferList[i]));

            for (int j = 0; j < this->renderBufferList[i].attributeInfos.size(); j++)
            {
                AttributeSegment& nowAttriInfo = this->renderBufferList[i].attributeInfos[j];
                if (nowAttriInfo.location != -1)
                {
                    for (int k = 0; k < nowAttriInfo.row; k++)
                        glVertexAttribPointer(nowAttriInfo.location + k, nowAttriInfo.col, nowAttriInfo.type, nowAttriInfo.normalized, nowAttriInfo.blockCount * nowAttriInfo.typeSize, (void*)((nowAttriInfo.offsetCount + k) * nowAttriInfo.typeSize));
                    glEnableVertexAttribArray(nowAttriInfo.location);
                }
            }
        }
    }
    void UpdateAll()
    {
        UpdateEBO();
        UpdateVBOAll();
    }
    void UpdateEBO()
    {
        if (activeIndexRender)
        {
            for (int i = 0; i < Shader::indexAttributeNameList.size(); i++)
                if (this->indexBuffer.FindAbleAttributeAsName((char*)Shader::indexAttributeNameList[i]))
                {
                    this->SetEBO(this->indexBuffer);
                    break;
                }
        }
    }
    void UpdateVBOAll()
    {
        for (int i = 0; i < this->renderBufferList.size(); i++)
            SetVBO(this->renderBufferList[i]);
    }
    void RenderingSetting()
    {
        if (material->para_zTest)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(material->para_zTestType);

            if (material->para_zWrite)
                glDepthMask(GL_TRUE);
            else
                glDepthMask(GL_FALSE);
        }
        else
            glDisable(GL_DEPTH_TEST);
        if (material->para_stencil)
        {
            glEnable(GL_STENCIL_TEST);
            glStencilMask(material->para_stencilMask);
            glStencilFunc(material->para_stencilType, 1, material->para_stencilMask);
            glStencilOp(material->para_stencil_sfail, material->para_stencil_dpfail, material->para_stencil_dppass);
        }
        else
            glDisable(GL_STENCIL_TEST);

        if (material->para_culling)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(material->para_cullingFace);
            glFrontFace(material->para_cullingCycle);
        }
        else
            glDisable(GL_BLEND);

        if (material->para_transparent)
        {
            glEnable(GL_BLEND);
            glBlendFunc(material->para_nextAlpha, material->para_prevAlpha);
        }
        else
            glDisable(GL_BLEND);


        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    void RenderingArray()
    {
        this->material->shader->Bind();
        BindVAO();
        int vertexCount = renderBufferList[0].size / renderBufferList[0].GetBlockCount();
        if (vertexCount % 3 != 0)
            WarringLog(std::cerr, "Warring") << "ArrayRender의 Count가 3의 배수가 아닙니다." << "\n";
        glDrawArrays(this->material->drawType, 0, vertexCount); //todo 수정 요망.
    }
    void RenderingIndex()
    {
        this->material->shader->Bind();
        BindVAO();
        if (this->activeIndexRender)
        {
            glDrawElements(this->material->drawType, indexBuffer.size / indexBuffer.GetBlockCount(), indexBuffer.glType, 0); //todo 수정 요망.
        }
        else
        {
            ErrorLog(std::cerr, "Error") << "Index Buffer가 존재하지 않는 Model을 Index 렌더링\n";
        }
    }
    void Rendering()
    {
        this->RenderingSetting();
        if (this->activeIndexRender)
            this->RenderingIndex();
        else
            this->RenderingArray();
    }

    void Destroy()
    {
        if (VAO != 0)
            glDeleteVertexArrays(1, &VAO);
        if (EBO != 0)
            glDeleteBuffers(1, &EBO);

        material = nullptr;
        modelData = nullptr;
    }
};

class ModelInfo
{
public:
    bool isInit = false;

    int connectShaderID = UINT32_MAX;
    unsigned int indexCount = 0;
    unsigned int arrayCount = 0;

    void* indexBufferAddress = nullptr;
    int indexBufferSize = 0;

    unsigned int VAO = UINT32_MAX;
    unsigned int EBO = UINT32_MAX;
    std::unordered_map<const char*, unsigned int, hash_str, equal_str> map_Attribute_VBO_IDs;
    std::unordered_map<const char*, VBOInfo, hash_str, equal_str> map_Attribute_VBO_Infos;

    void Bind()
    {
        if (VAO == UINT32_MAX)
            glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
    }

    Shader* SetShaderConnect(Shader* shader)
    {
        if (connectShaderID != shader->shaderID)
        {
            shader->Bind();
            this->Bind();

            if (shader != nullptr)
            {
                NormalLog(std::cerr, "Try") << "Material -> Shader Connect  shaderID : " << shader << "\n";
                bool result = UpdateAttrubute(shader);
                if (!result)
                {
                    if (shader->shaderID != errorShader.shaderID)
                        return SetShaderConnect(&errorShader); // Error shader 적용.
                    return nullptr;
                }
            }
            else
            {
                connectShaderID = UINT32_MAX;
                return nullptr;
            }
            connectShaderID = shader->shaderID;
        }
        return shader;
    }


    void Init()
    {
        if (!isInit)
        {
            if (VAO != UINT32_MAX)
                glDeleteVertexArrays(1, &VAO);

            if (EBO != UINT32_MAX)
                glDeleteBuffers(1, &EBO);
            for (auto it = map_Attribute_VBO_IDs.begin(); it != map_Attribute_VBO_IDs.end(); ++it)
                glDeleteBuffers(1, &(it->second));
            map_Attribute_VBO_IDs.clear();
            map_Attribute_VBO_Infos.clear();

            glGenVertexArrays(1, &VAO); //--- 버텍스 array 생성
            glBindVertexArray(VAO);
            glGenBuffers(1, &EBO);

            isInit = true;
        }
    }

    bool UpdateAttrubute(Shader* shader)
    {
        this->Bind();

        if (shader != nullptr)
        {

            for (int i = 0; i < shader->attributeInfos.size(); i++)
            {
                auto nowAttriInfo = shader->attributeInfos[i];

                if (map_Attribute_VBO_Infos.find(nowAttriInfo.name) == map_Attribute_VBO_Infos.end())
                {
                    ErrorLog(std::cerr, "Error") << "Material -> Shader Connected Failed : 쉐이더에 필요한 attribute를 찾을 수 없습니다. name : " << nowAttriInfo.name << "\n";
                    return false;
                }
                // 무조건 어트리뷰트 있음.
                VBOInfo VBO_Info = map_Attribute_VBO_Infos[nowAttriInfo.name];
                unsigned int VBO_Id = map_Attribute_VBO_IDs[nowAttriInfo.name];

                glBindBuffer(VBO_Info.bufferType, VBO_Id);
                for (int j = 0; j < nowAttriInfo.row; j++)
                    glVertexAttribPointer(nowAttriInfo.location + j, nowAttriInfo.col, nowAttriInfo.type, nowAttriInfo.normalized, VBO_Info.jumpSize, (void*)(VBO_Info.offset + j * Shader::GL_TypeToSizeOf(nowAttriInfo.type)));
                glEnableVertexAttribArray(nowAttriInfo.location);
            }
        }
        return true;
    }

    void SetArrayCount(unsigned int arrayCount)
    {
        this->arrayCount = arrayCount;
    }
    void SetIndexBuffer(unsigned int* indexBufferAddress, int arraySize)
    {
        Bind();
        if (EBO == UINT32_MAX)
            glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, arraySize, indexBufferAddress, GL_STATIC_DRAW);

        this->indexBufferAddress = indexBufferAddress;
        this->indexCount = arraySize / sizeof(unsigned int);
    }
    void SetBufferData(const char* attriName, VBOInfo info)
    {
        SetBufferData(attriName, info.dataBufferAddress, info.dataBufferSize, info.jumpSize, info.offset);
    }
    void SetBufferData(const char* attriName, void* dataBuffer, int dataBufferSize, unsigned int jumpSize, unsigned int offset)
    {
        unsigned int AttributeVBO_ID;
        auto VBO_Info = VBOInfo(dataBuffer, dataBufferSize, GL_ARRAY_BUFFER, jumpSize, offset);
        Bind();
        if (map_Attribute_VBO_IDs.find(attriName) != map_Attribute_VBO_IDs.end())
        {
            AttributeVBO_ID = map_Attribute_VBO_IDs[attriName];
            map_Attribute_VBO_Infos[attriName];
        }
        else
        {
            glGenBuffers(1, &AttributeVBO_ID);
            map_Attribute_VBO_IDs.insert({ attriName, AttributeVBO_ID });
            map_Attribute_VBO_Infos.insert({ attriName, VBO_Info });
        }
        map_Attribute_VBO_Infos[attriName] = VBO_Info;
        glBindBuffer(VBO_Info.bufferType, AttributeVBO_ID);
        glBufferData(VBO_Info.bufferType, dataBufferSize, dataBuffer, GL_STATIC_DRAW);
    }
    /*Buffer Address그대로고 데이터 내용 길이만 변할때.*/
    void UpdateDataBuffer(const char* attriName, void* dataBufferAddress = nullptr, int dataBufferSize = -1, int jumpSize = -1, int offset = -1)
    {
        Bind();
        if (map_Attribute_VBO_Infos.find(attriName) != map_Attribute_VBO_Infos.end())
        {
            auto bufferInfo = map_Attribute_VBO_Infos[attriName];
            if (dataBufferAddress != nullptr)
                bufferInfo.dataBufferAddress = dataBufferAddress;
            if (dataBufferSize != -1)
                bufferInfo.dataBufferSize = dataBufferSize;
            if (jumpSize != -1)
                bufferInfo.jumpSize = jumpSize;
            if (offset != -1)
                bufferInfo.offset = offset;

            SetBufferData(attriName, bufferInfo);
        }
    }
    void UpdateIndexBuffer(int indexBufferSize = -1)
    {
        Bind();
        if (EBO != UINT32_MAX)
        {
            if (indexBufferSize != -1)
            {
                this->indexBufferSize = indexBufferSize;
                this->indexCount = indexBufferSize / sizeof(unsigned int);
            }
            SetIndexBuffer((unsigned int*)this->indexBufferAddress, this->indexBufferSize);
        }
    }
};

std::shared_ptr<Model> Model::processModel(std::shared_ptr<aiScene> scene)
{
    std::shared_ptr<Model> model = std::shared_ptr<Model>(new Model());
    std::vector<std::shared_ptr<Mesh>> meshList;
    std::vector<std::shared_ptr<Animation>> animationList;

    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        auto mesh = scene->mMeshes[i];
        std::shared_ptr<Mesh> meshData = std::shared_ptr<Mesh>(new Mesh());
        meshList.push_back(meshData);

        // Init
        meshData->hasNormal = mesh->HasNormals();
        meshData->hasTangent = mesh->HasTangentsAndBitangents();
        for (int k = 0; k < 8; k++)
            meshData->hasColors[k] = mesh->HasVertexColors(k);
        for (int k = 0; k < 8; k++)
            meshData->hasUVs[k] = mesh->HasTextureCoords(k);
        meshData->hasBone = mesh->HasBones();

        // Vertex
        for (int j = 0; j < mesh->mNumVertices; j++)
        {
            MeshVertex vertex = {};

            vertex.pos = glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
            if (meshData->hasNormal)
                vertex.normal = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
            if (meshData->hasTangent)
                vertex.tangent = glm::vec3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z);
            for (int k = 0; k < 8; k++)
                if (meshData->hasColors[k])
                {
                    vertex.colors[k] = glm::vec4(
                        mesh->mColors[k][j].r,
                        mesh->mColors[k][j].g,
                        mesh->mColors[k][j].b,
                        mesh->mColors[k][j].a);
                    vertex.colorCount++;
                }
            for (int k = 0; k < 8; k++)
                if (meshData->hasUVs[k])
                {
                    vertex.uvs[k] = glm::vec2(
                        mesh->mTextureCoords[k][j].x,
                        mesh->mTextureCoords[k][j].y);
                    vertex.uvCount++;
                }
            meshData->vertexs.push_back(vertex);
        }

        // Index
        for (int j = 0; j < mesh->mNumFaces; j++)
        {
            meshData->faceAndindexs.push_back({});
            for (int k = 0; k < mesh->mFaces[j].mNumIndices; k++)
                meshData->faceAndindexs[j].push_back(mesh->mFaces[j].mIndices[k]);
        }

        // Index
        if (meshData->hasBone)
        {
            //std::cout << "매쉬당 본 갯수 : " << mesh->mNumBones << "\n";
            for (int j = 0; j < mesh->mNumBones; j++)
            {
                auto nowBone = mesh->mBones[j];
                int boneID = model->GetBoneID(nowBone);

                if (boneID != -1 && nowBone->mNumWeights == 0)
                {
                    //meshData->targetBoneId = boneID;
                    for (int k = 0; k < meshData->vertexs.size(); k++)
                    {
                        for (int n = 0; n < BoneMax; n++)
                        {
                            if (meshData->vertexs[k].boneIds[n] == -1)
                            {
                                meshData->vertexs[k].boneIds[n] = boneID;
                                meshData->vertexs[k].boneWeights[n] = 1.0f;
                                break;
                            }
                        }
                    }
                }

                for (int k = 0; k < nowBone->mNumWeights; k++)
                {
                    auto weightData = nowBone->mWeights[k];

                    auto weight = weightData.mWeight;
                    auto vertexId = weightData.mVertexId;
                    for (int n = 0; n < BoneMax; n++)
                    {
                        if (meshData->vertexs[vertexId].boneIds[n] == -1)
                        {

                            meshData->vertexs[vertexId].boneIds[n] = boneID;
                            meshData->vertexs[vertexId].boneWeights[n] = weight;
                            break;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < scene->mNumAnimations; i++)
    {
        std::shared_ptr<Animation> nowAnim = std::shared_ptr<Animation>(new Animation(model, scene.get(), scene->mAnimations[i]));
        animationList.push_back(nowAnim);
    }

    //return meshList;
    model->meshList = meshList;
    model->animationList = animationList;
    return model;
}

std::shared_ptr<ModelInfo2> Mesh::processData()
{
    std::vector<unsigned int> meshIndexList;
    std::vector<float> meshVertexList;

    std::vector<AttributeSegment> attri = { {"positionOS", 3} };
    if (hasNormal)
        attri.push_back({ "normalOS", 3 });
    if (hasTangent)
        attri.push_back({ "tangent", 3 });
    for (int k = 0; k < 8; k++)
        if (hasColors[k])
            attri.push_back({ ((std::string("vertexColor") + ((k == 0) ? std::string("") : std::to_string(k))).c_str()), 4 });
    for (int k = 0; k < 8; k++)
        if (hasUVs[k])
            attri.push_back({ (std::string("uv") + std::to_string(k)).c_str(), 2 });
    if (hasBone)
    {
        attri.push_back({ "boneIds", 4 });
        attri.push_back({ "boneWeights", 4 });
    }
    //for (int k = 0; k < attri.size(); k++)
    //    std::cout << attri[k].name << "\n";
    //, { "normalOS", 3 }, { "tangent", 3 }, { "bitangent", 3 }, { "vertexColor", 4 }, { "uv0", 2 }, { "uv1", 2 }, { "uv2", 2 }, { "uv3", 2 }

    for (int j = 0; j < (int)vertexs.size(); j++)
    {
        MeshVertex& vertex = vertexs[j];
        meshVertexList.push_back(vertex.pos.x);
        meshVertexList.push_back(vertex.pos.y); //이거 원상복구
        meshVertexList.push_back(vertex.pos.z);

        if (hasNormal)
        {
            meshVertexList.push_back(vertex.normal.x);
            meshVertexList.push_back(vertex.normal.y); //이거 원상복구
            meshVertexList.push_back(vertex.normal.z);
        }

        if (hasTangent)
        {
            meshVertexList.push_back(vertex.tangent.x);
            meshVertexList.push_back(vertex.tangent.y);
            meshVertexList.push_back(vertex.tangent.z);
        }

        for (int k = 0; k < 8; k++)
            if (hasColors[k]) //meshs->mColors 버텍스컬러 여러개 필요하면 나중에 추가
            {
                auto color = vertex.colors[k];
                meshVertexList.push_back(color.r);
                meshVertexList.push_back(color.g);
                meshVertexList.push_back(color.b);
                meshVertexList.push_back(color.a);
            }


        for (int k = 0; k < 8; k++)
        {
            if (hasUVs[k])
            {
                auto uv = vertex.uvs[k];
                meshVertexList.push_back(uv.x);
                meshVertexList.push_back(uv.y);
            }
        }
        if (hasBone)
        {
            auto ids = vertex.boneIds;
            for (int k = 0; k < BoneMax; k++)
                meshVertexList.push_back(ids[k]);
            auto weights = vertex.boneWeights;
            for (int k = 0; k < BoneMax; k++)
                meshVertexList.push_back(weights[k]);
        }
    }
    for (unsigned int j = 0; j < faceAndindexs.size(); ++j)
    {
        auto face = faceAndindexs[j];
        for (int k = 0; k < face.size(); k++)
            meshIndexList.push_back((unsigned int)face[k]);
    }
    auto meshVertexBuffer = new float[(int)meshVertexList.size()]();
    auto meshIndexBuffer = new unsigned int[(int)meshIndexList.size()]();

    int meshIndex = 0;
    for (int i = 0; i < (int)meshVertexList.size(); i++)
        meshVertexBuffer[meshIndex++] = meshVertexList[i];
    meshIndex = 0;
    for (int i = 0; i < (int)meshIndexList.size(); i++)
        meshIndexBuffer[meshIndex++] = meshIndexList[i];


    std::shared_ptr<ModelInfo2> modelData = std::shared_ptr<ModelInfo2>(new ModelInfo2({
        { std::shared_ptr<void>(meshVertexBuffer), (int)(meshVertexList.size()), GL_FLOAT, {
            attri
        }},
        { std::shared_ptr<void>(meshIndexBuffer), (int)(meshIndexList.size()), GL_UNSIGNED_INT, {{"vertexIndex", 1}}}
        }));
    return modelData;
}

std::vector<std::shared_ptr<ModelInfo2>> Loader::ConvertModelToModelDatas(std::shared_ptr<Model> model, std::shared_ptr<aiScene> aiScene)
{
    std::vector<std::shared_ptr<ModelInfo2>> modelList;

    std::vector<std::shared_ptr<Mesh>> meshList = model->meshList;
    for (int i = 0; i < meshList.size(); i++)
        modelList.push_back(meshList[i]->processData());
    return modelList;
}



Material::Material(Shader* shader)
{
    SetShader(shader);
    this->uniformStorage = std::shared_ptr<UniformStorage>(new UniformStorage());
}
void Material::SetShader(Shader* shader)
{
    this->shader = shader;
}



ShaderCode LoadShader(const char* path)
{
    ShaderCode shaderCode;
    shaderCode.path = path;
    shaderCode.code = filetobuf(path);
    shaderCode.isShaderLoad = false;
    if (shaderCode.code != nullptr)
        shaderCode.isShaderLoad = true;
    NormalLog(std::cerr, "Try") << "Shader Load Success - Path : \"" << path << "\"\n";
    return shaderCode;
}
ShaderCode&& CompileShader(ShaderCode&& shader, unsigned int ShaderType)
{
    shader.shaderType = ShaderType;
    shader.shaderCodeID = glCreateShader(shader.shaderType);
    glShaderSource(shader.shaderCodeID, 1, &(shader.code), NULL);
    glCompileShader(shader.shaderCodeID);

    int result;
    glGetShaderiv(shader.shaderCodeID, GL_COMPILE_STATUS, &result); // 결과 보기
    if (!result)
    {
        char errorLog[8192];
        glGetShaderInfoLog(shader.shaderCodeID, 8192, NULL, errorLog); // 에러가 뭔지 보기
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        ErrorLog(std::cerr, "Error", 1) << "Shader 컴파일 실패, Type : " << ((shader.shaderType == GL_VERTEX_SHADER) ? "Vertex Shader" : "Fragment Shader") << "\n" << errorLog << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

        int index = 0;
        while (errorLog[index] != '\0')
            if (errorLog[index] == '(')
                break;
            else
                index++;
        int lineNumber = 0;
        while (errorLog[++index] != ')')
            lineNumber = lineNumber * 10 + (int)(errorLog[index] - '0');
        index = 0;
        while (shader.code[index] != '\0')
        {
            int prevIndex = index;
            if (index == 0 || shader.code[index] == '\n')
            {
                lineNumber--;
                if (lineNumber == 0)
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
                else
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            }
            std::cerr << shader.code[index];
            index++;
        }std::cerr << "\n";

        shader.isShaderCompiled = false;
        return std::move(shader);
    }
    shader.isShaderCompiled = true;
    NormalLog(std::cerr, "Try", 1) << "Shader Compile Success - ShaderCodeID : " << shader.shaderCodeID << ", Type:" << ((shader.shaderType == GL_VERTEX_SHADER) ? "Vertex Shader" : "Fragment Shader") << "\n";
    return std::move(shader);
}

Shader CreateShaderProgram(ShaderCode&& vertexShaderCode, ShaderCode&& fragmentShaderCode)
{
    if (!(vertexShaderCode.isShaderLoad && vertexShaderCode.isShaderCompiled))
    {
        ErrorLog(std::cerr, "Error", 2) << "Program 생성 실패 : 잘못된 VertexShader" << (vertexShaderCode.isShaderLoad ? " - Load 되지 않은 Shader" : "") << (vertexShaderCode.isShaderCompiled ? " - Compile 실패한 Shader" : "") << "\n";
        ErrorLog(std::cerr, "Error", 2) << "Shader Compile 실패 -> Error Shader 반환" << "\n";
        vertexShaderCode.Delete();
        fragmentShaderCode.Delete();
        return errorShader;
    }
    if (!(vertexShaderCode.isShaderLoad && vertexShaderCode.isShaderCompiled))
    {
        ErrorLog(std::cerr, "Error", 2) << "Program 생성 실패 : 잘못된 FragmentShader" << (fragmentShaderCode.isShaderLoad ? " - Load 되지 않은 Shader" : "") << (fragmentShaderCode.isShaderCompiled ? " - Compile 실패한 Shader" : "") << "\n";
        ErrorLog(std::cerr, "Error", 2) << "Shader Compile 실패 -> Error Shader 반환" << "\n";
        vertexShaderCode.Delete();
        fragmentShaderCode.Delete();
        return errorShader;
    }


    Shader newShader;
    newShader.shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기
    glAttachShader(newShader.shaderID, vertexShaderCode.shaderCodeID); //--- 세이더 프로그램에 버텍스 세이더 붙이기
    glAttachShader(newShader.shaderID, fragmentShaderCode.shaderCodeID); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
    glLinkProgram(newShader.shaderID); //gl에 링크하는건가봄

    int result;
    glGetProgramiv(newShader.shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        char errorLog[8192];
        glGetProgramInfoLog(newShader.shaderID, 8192, NULL, errorLog);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        ErrorLog(std::cerr, "Error", 2) << "Program 연결 실패\n " << errorLog << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        ErrorLog(std::cerr, "Error", 2) << "Shader Compile 실패 -> Error Shader 반환" << "\n";
        vertexShaderCode.Delete();
        fragmentShaderCode.Delete();
        newShader.Delete();
        return errorShader;
    }

    NormalLog(std::cerr, "Try", 2) << "Shader Program Create Completed - ShaderProgramCode : " << newShader.shaderID << ",  Path : \"" << vertexShaderCode.path << "\" / \"" << fragmentShaderCode.path << "\"\n";
    vertexShaderCode.Delete();
    fragmentShaderCode.Delete();

    newShader.isCompleted = true;
    newShader.vertexCode = vertexShaderCode;
    newShader.fragmentCode = fragmentShaderCode;
    newShader.Init();
    newShader.Bind();
    return newShader;
}


unsigned int LoadTextureImage(const char* imageDir)
{
    // 1. Load Image
    int imageWidth, imageHeight;
    unsigned char* image = SOIL_load_image(imageDir,
        &imageWidth, &imageHeight, NULL, SOIL_LOAD_RGBA);

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 3. Setup Options
    // UV 벗어날 경우 텍스쳐 반복
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 텍스쳐 축소/확대 필터 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // 4. Generate Texture2D
    if (image)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        ErrorLog(std::cerr, "Error", 0) << "Texture Load 실패 - " << imageDir << "\n";
    }

    SOIL_free_image_data(image); // Release image

    return textureID;
}


#pragma endregion


//template<> bool GameObject::RemoveComponent(std::weak_ptr<Component> element);
//template<> std::weak_ptr<Component> GameObject::AddComponent(Component* element);
//template<> std::weak_ptr<Component> GameObject::GetComponent();
//template<> void GameObject::DestroyComponent(std::weak_ptr<Component> element);


World::World()
{
    uniformStorage = std::shared_ptr<UniformStorage>(new UniformStorage());
}


void World::Init()
{
    this->mainCameraObject = this->CreateGameObject("mainCamera");
    this->mainCamera = this->mainCameraObject.lock()->AddComponent<Camera>(new Camera());
}

std::weak_ptr<GameObject> World::CreateGameObject()
{
    return this->CreateGameObject("object");
}
std::weak_ptr<GameObject> World::CreateGameObject(const char* name)
{
    std::shared_ptr<GameObject> gameObject;
    this->AddGameObject(gameObject = std::shared_ptr<GameObject>(new GameObject((char*)name)));
    gameObject->Create();
    return gameObject;
}

bool World::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
    for (int i = 0; i < gameObjectList.size(); i++)
        if (gameObjectList[i] == gameObject)
            return false;
    gameObjectList.push_back(gameObject);
    gameObject->world = shared_from_this();
    return true;
}
void World::WorldUpdate()
{
    for (int i = 0; i < gameObjectList.size(); i++)
        gameObjectList[i]->Enable();
    for (int i = 0; i < gameObjectList.size(); i++)
        gameObjectList[i]->Start();
    for (int i = 0; i < gameObjectList.size(); i++)
        gameObjectList[i]->Update();
    for (int i = 0; i < gameObjectList.size(); i++)
        gameObjectList[i]->LateUpdate();
    for (int i = 0; i < gameObjectList.size(); i++)
        gameObjectList[i]->Disable();
    for (int i = 0; i < gameObjectList.size(); i++)
        gameObjectList[i]->PostBehavior();
    for (int i = 0; i < gameObjectList.size(); i++)
        if (gameObjectList[i]->destroy)
            gameObjectList.erase(gameObjectList.begin() + (i--));
}
void World::WorldRender()
{
    renderingList.clear();
    for (int i = 0; i < gameObjectList.size(); i++)
        gameObjectList[i]->BeforeRender();
    std::sort(renderingList.begin(), renderingList.end(), [](const std::shared_ptr<RenderData>& a, const std::shared_ptr<RenderData>& b)
        {
            float aWeight = a->zSorting + 10000 * (a->material->para_transparent ? 1 : 0);
            float bWeight = b->zSorting + 10000 * (b->material->para_transparent ? 1 : 0);
            return aWeight < bWeight;
        });
    for (int i = 0; i < renderingList.size(); i++)
    {
        std::shared_ptr<RenderData> renderData = renderingList[i];
        Shader* nowShader = renderData->material->shader;
        std::shared_ptr<UniformStorage> shaderUniformStorage = nowShader->uniformStorage;
        
        nowShader->Bind();
        shaderUniformStorage->UpdateUniforms(*uniformStorage.get());
        shaderUniformStorage->UpdateUniforms(*renderData->uniformStorage.get());
        shaderUniformStorage->UpdateUniforms(*renderData->material->uniformStorage.get());
        for (int j = 0; j < shaderUniformStorage->uniformList.size(); j++)
            shaderUniformStorage->uniformList[j].Bind();
        renderData->Rendering();
    }
}


glm::mat4 Transform::GetModelToWorld()
{
    if (positionPrev.x != position.x || positionPrev.y != position.y || positionPrev.z != position.z ||
        rotationPrev.x != rotation.x || rotationPrev.y != rotation.y || rotationPrev.z != rotation.z ||
        scalePrev.x != scale.x || scalePrev.y != scale.y || scalePrev.z != scale.z)
    {
        TRSCache = glm::mat4(1);
        TRSCache = glm::translate(TRSCache, this->position);
        TRSCache = glm::rotate(TRSCache, this->rotation.y, glm::vec3(0, 1, 0));
        TRSCache = glm::rotate(TRSCache, this->rotation.x, glm::vec3(1, 0, 0));
        TRSCache = glm::rotate(TRSCache, this->rotation.z, glm::vec3(0, 0, 1));
        TRSCache = glm::scale(TRSCache, this->scale);

        this->positionPrev = this->position;
        this->rotationPrev = this->rotation;
        this->scalePrev = this->scale;
    }
    return TRSCache;
}
glm::mat4 Transform::GetWorldToModel()
{
    return glm::inverse(TRSCache);
}

glm::mat4&& Transform::GetModelToWorldAll()
{
    auto tempNow = this;
    auto TotalTRS = glm::mat4(1);
    while (tempNow != nullptr)
    {
        if (!tempNow->gameObject.expired())
        {
            TotalTRS = tempNow->GetModelToWorld() * TotalTRS;
            if ((!tempNow->gameObject.lock()->parent.expired()) && (!tempNow->gameObject.lock()->parent.lock()->transform.expired()))
            {
                tempNow = tempNow->gameObject.lock()->parent.lock()->transform.lock().get();
            }
            else
                tempNow = nullptr;
        }
        else
            tempNow = nullptr;
    }
    return std::move(TotalTRS);
}
glm::mat4&& Transform::GetWorldToModelAll()
{
    return glm::inverse(this->GetModelToWorldAll());
}



GameObject::GameObject()
{
    active = true;
    this->name = std::shared_ptr<char>(new char[7]{'o','b','j','e','c','t', 0});
}
GameObject::GameObject(char* name)
{
    active = true;
    int nameLength = strlen(name);
    char* tempName;
    strncpy((tempName = new char[nameLength + 1]()), name, nameLength);
    this->name = std::shared_ptr<char>(tempName);
}

void GameObject::Create()
{
    this->transform = this->AddComponent(new Transform());
}
void GameObject::Enable()
{
    for (int i = 0; i < componentList.size(); i++)
    {
        if (componentList[i] != nullptr)
        {
            if (((this->active) && this->active != this->activePrev) ||
                (componentList[i]->active && componentList[i]->active != componentList[i]->activePrev))
            {
                componentList[i]->Enable();
                componentList[i]->activePrev = componentList[i]->active;
            }
        }
    }
    this->activePrev = this->active;
}

void GameObject::Disable()
{
    for (int i = 0; i < componentList.size(); i++)
    {
        if (componentList[i] != nullptr)
        {
            if (this->destroy)
                componentList[i]->destroy = true;
            if (componentList[i]->destroy && componentList[i]->active)
                componentList[i]->active = false;

            if (((!this->active) && this->active != this->activePrev) ||
                ((!componentList[i]->active) && componentList[i]->active != componentList[i]->activePrev))
            {
                componentList[i]->Disable();
                componentList[i]->activePrev = componentList[i]->active;
            }
        }
    }
    this->activePrev = this->active;
}

void GameObject::Start()
{
    if (this->active)
    {
        for (int i = 0; i < componentList.size(); i++)
        {
            if (componentList[i] != nullptr)
            {
                if (componentList[i]->active)
                {
                    if (componentList[i]->isFirstUpdate)
                    {
                        componentList[i]->Start();
                        componentList[i]->isFirstUpdate = false;
                    }
                }
            }
        }
    }
}

void GameObject::Update()
{

    if (this->active)
    {
        for (int i = 0; i < componentList.size(); i++)
        {
            if (componentList[i] != nullptr) // 컴포넌트가 Null이 아니면서, 
            {
                if (componentList[i]->active)
                {
                    componentList[i]->Update();
                }
            }
        }
    }
}
void GameObject::LateUpdate()
{

    if (this->active)
    {
        for (int i = 0; i < componentList.size(); i++)
        {
            if (componentList[i] != nullptr) // 컴포넌트가 Null이 아니면서, 
            {
                if (componentList[i]->active)
                {
                    componentList[i]->LateUpdate();
                }
            }
        }
    }
}


void GameObject::PostBehavior()
{
    for (int i = 0; i < componentList.size(); i++)
    {
        if (componentList[i] != nullptr)
        {
            if (this->destroy)
                componentList[i]->destroy = true;
            if (componentList[i]->destroy)
            {
                componentList[i]->Destroy();
            }
        }
    }
    this->ClearComponent();

}
template <class T>
void GameObject::DestroyComponent(T* element)
{
    Component* castingComponent = nullptr;
    for (int i = 0; i < this->componentList.size(); i++)
    {
        std::shared_ptr<T> castingComponent = std::dynamic_pointer_cast<T>(this->componentList[i]);
        if (castingComponent != nullptr && castingComponent.get() == element)
        {
            castingComponent->destroy = true;
        }
    }
}

void GameObject::BeforeRender()
{
    if (this->active)
    {
        for (int i = 0; i < componentList.size(); i++)
        {
            if (componentList[i] != nullptr) // 컴포넌트가 Null이 아니면서, 
            {
                if (componentList[i]->active && !componentList[i]->destroy)
                {
                    componentList[i]->BeforeRender();
                }
            }
        }
    }
}


template <class T>
std::weak_ptr<T> GameObject::AddComponent(T* component)
{
    std::shared_ptr<T> componentShared = std::shared_ptr<T>(component);
    std::shared_ptr<Component> castingComponent = std::dynamic_pointer_cast<Component>(componentShared);
    if (castingComponent != nullptr)
    {
        componentList.push_back(castingComponent);
        castingComponent->gameObject = shared_from_this();
        castingComponent->Create();
    }
    return std::weak_ptr<T>(componentShared);
}

template <class T>
std::weak_ptr<T> GameObject::GetComponent()
{
    Component* castingComponent = nullptr;
    for (int i = 0; i < this->componentList.size(); i++)
    {
        auto castingComponent = std::dynamic_pointer_cast<T>(this->componentList[i]);
        if (castingComponent != nullptr)
        {
            return castingComponent;
        }
    }
    return castingComponent;
}
template <class T>
bool GameObject::RemoveComponent(T* element)
{
    for (int i = 0; i < this->componentList.size(); i++)
    {
        std::shared_ptr<T> castingComponent = std::dynamic_pointer_cast<T>(this->componentList[i]);
        if (castingComponent != nullptr && castingComponent.get() == element)
        {
            return GameObject::RemoveAtComponent(i);
        }
    }
    return false;
}
bool GameObject::RemoveAtComponent(int index)
{
    this->componentList[index] = nullptr;
    return true;
}

void GameObject::ClearComponent()
{
    for (int i = 0; i < this->componentList.size(); i++)
    {
        if (this->componentList[i] == nullptr || this->componentList[i]->destroy)
        {
            this->componentList.erase(this->componentList.begin() + i);
            i--;
        }
    }
}

void GameObject::AddChild(GameObject* child)
{
    child->SetParent(shared_from_this());
}

bool GameObject::SetParent(std::weak_ptr<GameObject> parent)
{
    auto parentCheck = parent.lock().get();
    bool parentIsMyChild = false;
    while (parentCheck != nullptr)
    {
        if (parentCheck == this)
        {
            parentIsMyChild = true;
            break;
        }
        parentCheck = !parentCheck->parent.expired() ? parentCheck->parent.lock().get() : nullptr;
    }
    if (parentIsMyChild)
    {
        WarringLog(std::cerr, "Warring") << "Failed SetParent" << " - 부모가 자신의 자식들에 속함.\n";
        return false;
    }
    if (!this->parent.expired() && this->parent.lock() != nullptr)
    {
        for (int i = 0; i < this->parent.lock()->childs.size(); i++)
        {
            if (this->parent.lock()->childs[i].lock().get() == this)
            {
                this->parent.lock()->childs.erase(this->parent.lock()->childs.begin() + i);
                i--;
            }
        }
    }
    this->parent = parent;
    this->parent.lock()->childs.push_back(shared_from_this());
    return true;
}



void Camera::BeforeRender()
{
    glm::vec4 worldForward = this->gameObject.lock()->transform.lock()->GetModelToWorldAll() * glm::vec4(0, 0, 1, 0);
    this->aspect = CorePipeline::screenW / CorePipeline::screenH;

    auto viewMatrix = glm::lookAt(this->gameObject.lock()->transform.lock()->position, this->gameObject.lock()->transform.lock()->position + glm::vec3(worldForward), glm::vec3(0, 1.0f, 0));
    auto projectionMatrix = glm::perspective(fovy * D2R, this->aspect, nearValue, farValue);

    this->gameObject.lock()->world.lock()->uniformStorage->PushUniform(UniformSegment("matrix_View", GL_FLOAT_MAT4).SetData(viewMatrix));
    this->gameObject.lock()->world.lock()->uniformStorage->PushUniform(UniformSegment("matrix_Projection", GL_FLOAT_MAT4).SetData(projectionMatrix));
    this->gameObject.lock()->world.lock()->uniformStorage->PushUniform(UniformSegment("matrix_ViewProjection", GL_FLOAT_MAT4).SetData(projectionMatrix * viewMatrix));
}


void ModelRenderer::AddRenderData(RenderData* renderData)
{
    int index = -1;
    for (int i = 0; i < this->renderDataList.size(); i++)
        if (this->renderDataList[i]->material == renderData->material &&
            this->renderDataList[i]->modelData == renderData->modelData)
            index = i;
    if(index == -1)
        this->renderDataList.push_back(std::shared_ptr<RenderData>(renderData));
}
void ModelRenderer::AddRenderData(std::shared_ptr<RenderData> renderData)
{
    int index = -1;
    for (int i = 0; i < this->renderDataList.size(); i++)
        if (this->renderDataList[i]->material == renderData->material &&
            this->renderDataList[i]->modelData == renderData->modelData)
            index = i;
    if (index == -1)
        this->renderDataList.push_back(renderData);
}
void ModelRenderer::AddModel(ModelInfo2* modelInfo)
{
    for (int i = 0; i < this->materialList.size(); i++)
    {
        auto renderData = std::shared_ptr<RenderData>(new RenderData(this->materialList[i], modelInfo));
        this->renderDataList.push_back(renderData);
    }
    this->modelList.push_back(modelInfo);
}
void ModelRenderer::AddMaterial(Material* material)
{
    for (int i = 0; i < this->modelList.size(); i++)
    {
        auto renderData = std::shared_ptr<RenderData>(new RenderData(material, this->modelList[i]));
        this->renderDataList.push_back(renderData);
    }
    this->materialList.push_back(material);
}
void ModelRenderer::AddMaterialModel(Material* material, ModelInfo2* modelInfo)
{
    this->AddMaterial(material);
    this->AddModel(modelInfo);
}
void ModelRenderer::BeforeRender()
{
    for (int i = 0; i < this->renderDataList.size(); i++)
    {
        std::shared_ptr<RenderData> nowRenderData = this->renderDataList[i];
        nowRenderData->uniformStorage->PushUniform(UniformSegment("matrix_ModelToWorld", GL_FLOAT_MAT4).SetData(this->gameObject.lock()->transform.lock()->GetModelToWorldAll()));
        //nowRenderData->material->uniformStorage->PushUniform(UniformSegment("matrix_ModelToWorld", GL_FLOAT_MAT4).SetData(this->gameObject.lock()->transform.lock()->GetModelToWorldAll()));
        //this->renderData->material->uniformStorage->PushUniform(UniformSegment("_MainLightDirection", GL_FLOAT_VEC3).SetData(glm::normalize(glm::vec3(light->transform.lock()->position) - glm::vec3(this->gameObject.lock()->transform.lock()->position))));
        //this->renderData->material->uniformStorage->PushUniform(UniformSegment("_ViewDirection", GL_FLOAT_VEC3).SetData(glm::normalize(cameraPos - this->gameObject.lock()->transform.lock()->position)));
        //this->renderData->material->uniformStorage->PushUniform(UniformSegment("MainLightPower", GL_FLOAT).SetData(lightPow));
        gameObject.lock()->world.lock()->renderingList.push_back(nowRenderData);
    }
}

void CameraControl::Start()
{
    offsetPosition = glm::vec3(0, 1.0f, 0);
    offsetDistance = 3.0f;
}

void CameraControl::Update()
{
    if (auto target = targetObject.lock())
    {
        targetPosition = target->transform.lock()->position;
    }

    glm::vec3 finalTargetPosition = targetPosition + offsetPosition;
    glm::vec3 finalDirection = glm::vec3(
        cos(directionAngle.y) * cos(directionAngle.x),
        sin(directionAngle.x),
        sin(directionAngle.y) * cos(directionAngle.x));
    finalDirection = glm::normalize(finalDirection) * offsetDistance;
    //directionAngle.y += 0.01f;
    this->gameObject.lock()->transform.lock()->position = finalTargetPosition + finalDirection;
    glm::vec3 angle = glm::eulerAngles(glm::quatLookAtRH(glm::normalize(-finalDirection), glm::vec3(0, 1, 0)));
    
    directionAngle.y += 0.01f;
    this->gameObject.lock()->transform.lock()->rotation = glm::vec3(directionAngle.x,
        -directionAngle.y - 90 * D2R, 0);// * D2R
}


GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Update(int updateID);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void MouseWheel(int button, int dir, int x, int y);
void KeyboardDown(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);
void KeyboardSpecDown(int key, int x, int y);
void KeyboardSpecUp(int key, int x, int y);
void Keyboard(int key, bool spec, int state, int x, int y);


//----new system----
Shader testShader;
Shader animShader;
ModelInfo* testModel;
Material* testMaterial;
RenderData* testRenderData;

float testTotalVertexBuffer[32] = { 0, 0, 0, 1, 0, 1, 0, 1,
               0.5, 0, 0, 1,  1, 1, 0, 1,
               0.5, -0.5, 0, 1, 1, 0, 0, 1,
               0, -0.5, 0, 1, 0, 0, 0, 1 };
unsigned int testIndexs[6] = { 0, 3, 2,
                        0, 2, 1 };


std::vector<RenderData*> klee;

std::shared_ptr<Animator> animator;
std::shared_ptr<Model> model;


std::shared_ptr<World> world;

std::shared_ptr<GameObject> A;
std::shared_ptr<GameObject> B;
std::shared_ptr<GameObject> C;

float mouseX = 0;
float mouseY = 0;
float angleX = 0;
float angleY = 0;
bool pushZ = false;

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 { //--- 윈도우 생성하기
{
    SetWindowPos(GetConsoleWindow(), 0, 0, 0, 1500, 800, SWP_SHOWWINDOW);
    SetConsoleTitle(L"콘솔");
    std::cout.setf(std::ios::right);

    timeBeginPeriod(1);
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    glutInit(&argc, argv); // glut 초기화
#ifdef __FREEGLUT_EXT_H__
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH); // 디스플레이 모드 설정
    glutInitWindowPosition(310, 180); // 윈도우의 위치 지정
    glutInitWindowSize(CorePipeline::screenW, CorePipeline::screenH); // 윈도우의 크기 지정
    //glutFullScreen();
    glutCreateWindow("GLEW_1"); // 윈도우 생성

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) // glew 초기화
    {
        ErrorLog(std::cerr, "Error") << "Unable to initialize GLEW\n";
        exit(EXIT_FAILURE);
    }
    else
        NormalLog(std::cerr, "Log") << "GLEW Init Completed\n";
    splitLine();

    //----------------------------------------------------


    errorShader = CreateShaderProgram(
        CompileShader(LoadShader("./Shaders/gl_Error_Vert.glsl"), GL_VERTEX_SHADER),
        CompileShader(LoadShader("./Shaders/gl_Error_Frag.glsl"), GL_FRAGMENT_SHADER));
    //errorShader.SetAttribute("positionOS", 4, 1, GL_FLOAT_VEC4, GL_FLOAT, GL_FALSE);
    splitLine();

    testShader = CreateShaderProgram(
        CompileShader(LoadShader("./Shaders/gl_vertex.glsl"), GL_VERTEX_SHADER),
        CompileShader(LoadShader("./Shaders/gl_fragment.glsl"), GL_FRAGMENT_SHADER));
    
    animShader = CreateShaderProgram(
        CompileShader(LoadShader("./Shaders/gl_Anim_vertex.glsl"), GL_VERTEX_SHADER),
        CompileShader(LoadShader("./Shaders/gl_Anim_fragment.glsl"), GL_FRAGMENT_SHADER));

    splitLine();


    world = std::shared_ptr<World>(new World());
    world->Init();
    /*
    uniform mat4 matM;
    uniform mat4 matrix_ModelToWorld; // model -> world
    uniform mat4 matrix_WorldToModel; // world -> model
    uniform mat4 matrix_ViewProjection; // world -> clip
    uniform mat4 matrix_View; // world -> view
    uniform mat4 matrix_Projection; // view -> clip
    */
    int textureId = (int)LoadTextureImage("./Models/Klee/Avatar_Loli_Catalyst_KleeCostumeWitch_Tex_Hair_Diffuse.png");
    testMaterial = new Material(&animShader);
    testMaterial->drawType = GL_TRIANGLES;
    testMaterial->uniformStorage->PushUniform(UniformSegment("_MainTex", GL_SAMPLER_2D).SetData(textureId));

    auto testBodyMaterial = new Material(&animShader);
    testBodyMaterial->drawType = GL_TRIANGLES;
    testBodyMaterial->uniformStorage->PushUniform(UniformSegment("_MainTex", GL_SAMPLER_2D).SetData((int)LoadTextureImage("./Models/Klee/Avatar_Loli_Catalyst_KleeCostumeWitch_Tex_Body_Diffuse.png")));
    
    auto testFaceMaterial = new Material(&animShader);
    testFaceMaterial->drawType = GL_TRIANGLES;
    testFaceMaterial->uniformStorage->PushUniform(UniformSegment("_MainTex", GL_SAMPLER_2D).SetData((int)LoadTextureImage("./Models/Klee/Avatar_Loli_Catalyst_KleeCostumeWitch_Tex_Face_Diffuse.png")));

    
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile("./Models/Klee/Avatar_Loli_Catalyst_KleeCostumeWitch.fbx",
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        //aiProcess_GenNormals |
        aiProcess_OptimizeMeshes |// 매쉬 다 합치는거
        //aiProcess_SplitLargeMeshes // 매쉬가 너무 클때 쪼개는거
        //aiProcess_ImproveCacheLocality | 삼각형 개선. 잘 되면 켜보기
        //aiProcess_GenUVCoords | UV없으면 UV 계산하게[ 시키기
        //aiProcess_Debone | 손실없이 뼈 제거.
        aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_SortByPType);

    NormalLog(std::cerr, "Try") << "Mesh Load - Klee (mesh count)" << pScene->mNumMeshes << "\n";
    /* fast 퀄리티
    aiProcess_CalcTangentSpace              |  \
        aiProcess_GenNormals                    |  \
        aiProcess_JoinIdenticalVertices |  \
        aiProcess_Triangulate                   |  \
        aiProcess_GenUVCoords           |  \
        aiProcess_SortByPType           |  \
        0
    */
    /* 왼손으로 변환
    ( \
        aiProcess_MakeLeftHanded     | \
        aiProcess_FlipUVs            | \
        aiProcess_FlipWindingOrder   | \
        0 )
    */
    /*max 퀄리티
    ( \
        aiProcessPreset_TargetRealtime_Quality   |  \
        aiProcess_FindInstances                  |  \
        aiProcess_ValidateDataStructure          |  \
        aiProcess_OptimizeMeshes                 |  \
        aiProcess_Debone                                                 |  \
        0 )
    */


    //testModel = new ModelInfo();
    //testModel->Init();
    //testModel->SetIndexBuffer(testIndexs, sizeof(testIndexs));
    //testModel->SetArrayCount(3);
    //testModel->SetBufferData("positionOS", testTotalVertexBuffer, sizeof(testTotalVertexBuffer), 8 * sizeof(float), 0); // todo : 이부분 다시 체크 3* sizeof(float)로
    //testModel->SetBufferData("vertexColor", testTotalVertexBuffer, sizeof(testTotalVertexBuffer), 8 * sizeof(float), 4 * sizeof(float));

    /*
    float* meshVertexBuffer;
    unsigned int* meshIndexBuffer;
    std::vector<unsigned int> meshIndexList;
    std::vector<float> meshVertexList;

    A = world->CreateGameObject().lock();
    for (int k = 0; k < pScene->mNumMeshes; k++)//pScene->mNumMeshes
    {
        for (unsigned int i = 0; i < pScene->mMeshes[k]->mNumVertices; ++i)
        {
            meshVertexList.push_back((pScene->mMeshes[k]->mVertices[i]).x);
            meshVertexList.push_back((pScene->mMeshes[k]->mVertices[i]).y);
            meshVertexList.push_back((pScene->mMeshes[k]->mVertices[i]).z);
            meshVertexList.push_back(1.0f);
            meshVertexList.push_back((pScene->mMeshes[k]->mNormals[i]).x);
            meshVertexList.push_back((pScene->mMeshes[k]->mNormals[i]).y);
            meshVertexList.push_back((pScene->mMeshes[k]->mNormals[i]).z);
            meshVertexList.push_back(1.0f);
            meshVertexList.push_back((pScene->mMeshes[k]->mTextureCoords[0][i]).x);
            meshVertexList.push_back((pScene->mMeshes[k]->mTextureCoords[0][i]).y);
        }
        for (unsigned int i = 0; i < pScene->mMeshes[k]->mNumFaces; ++i)
        {
            auto face = pScene->mMeshes[k]->mFaces[i];
            for (int j = 0; j < face.mNumIndices; j++)
                meshIndexList.push_back((face.mIndices[j]));
        }
        meshVertexBuffer = new float[(int)meshVertexList.size()]();
        meshIndexBuffer = new unsigned int[(int)meshIndexList.size()]();

        int meshIndex = 0;
        for (int i = 0; i < (int)meshIndexList.size(); i++)
            meshIndexBuffer[meshIndex++] = meshIndexList[i];
        meshIndex = 0;
        for (int i = 0; i < (int)meshVertexList.size(); i++)
            meshVertexBuffer[meshIndex++] = meshVertexList[i];
        ModelInfo2* m = new ModelInfo2({
            { std::shared_ptr<void>(meshVertexBuffer), (int)(meshVertexList.size()), GL_FLOAT, {{"positionOS", 4}, {"vertexColor", 4}, {"uv0", 2} } },
            { std::shared_ptr<void>(meshIndexBuffer), (int)(meshIndexList.size()), GL_UNSIGNED_INT, {{"vertexIndex", 1}}}
            });
        if (k == 0)
        {
            auto modelRenderer1 = A->AddComponent<ModelRenderer>(new ModelRenderer);
            modelRenderer1.lock()->AddMaterialModel(testMaterial, m);
        }
        if (k == 1)
        {
            auto modelRenderer1 = A->AddComponent<ModelRenderer>(new ModelRenderer);
            modelRenderer1.lock()->AddMaterialModel(testBodyMaterial, m);
        }
        //if (k == 5)
        //    klee.push_back(new RenderData(testFaceMaterial, m));
        if (k == 3 || k == 6 || k == 7)
        {
            auto modelRenderer1 = A->AddComponent<ModelRenderer>(new ModelRenderer);
            modelRenderer1.lock()->AddMaterialModel(testFaceMaterial, m);
        }
        //else if (k == 4 || k == 5)
        //    klee.push_back(new RenderData(testFaceMaterial, m));
        //else
        //    klee.push_back(new RenderData(testMaterial, m));
        meshIndexList.clear();
        meshVertexList.clear();
    }
    */


    Assimp::Importer aiImporter;
    std::shared_ptr<aiScene> aiScene;
    aiScene = Loader::ModelLoad("./Models/Anbi/Avatar_Female_Size02_Anbi.fbx", aiImporter);
    model = Model::processModel(aiScene);
    animator = std::shared_ptr<Animator>(new Animator(model->animationList[33]));
    animator->Update(CorePipeline::deltaTime);

    auto list = Loader::ConvertModelToModelDatas(model, aiScene);


    C = world->CreateGameObject("Anbi").lock();
    for (int i = 0; i < list.size(); i++)
    {
        B = world->CreateGameObject().lock();
        auto modelRenderer1 = B->AddComponent<ModelRenderer>(new ModelRenderer);
        Material* material = new Material(&animShader);
        material->uniformStorage->PushUniform(UniformSegment("_MainTex", GL_SAMPLER_2D).SetData(textureId));
        modelRenderer1.lock()->AddMaterialModel(material, list[i].get());
        B->transform.lock()->rotation = glm::vec3(0, 0, 0);
        B->SetParent(C);
        //if(list[i]->)
    }

    //world->mainCameraObject.lock()->transform.lock()->position = glm::vec3(0, 1.5, -2);
    auto cameraControl = world->mainCameraObject.lock()->AddComponent<CameraControl>(new CameraControl());
    

    ImGUIInit();

    glutDisplayFunc(drawScene); // 출력 함수의 지정
    glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutPassiveMotionFunc(Motion);
    glutMouseWheelFunc(MouseWheel);
    glutKeyboardFunc(KeyboardDown);
    glutKeyboardUpFunc(KeyboardUp);
    glutSpecialFunc(KeyboardSpecDown);
    glutSpecialUpFunc(KeyboardSpecUp);

    glutTimerFunc(1, Update, 16);

    //----------------------------------------------------

    //glutMainLoop(); // 이벤트 처리 시작
    //glutLeaveMainLoop()

    FrameInit(10);

    CorePipeline::updateNowClock = std::chrono::steady_clock::now();
    CorePipeline::updatePrevClock = CorePipeline::updateNowClock;
    while (!CorePipeline::isGameExit)
    {
        CorePipeline::targetFrameBetween = ((float)1000) / CorePipeline::targetFrame;
        CorePipeline::updateNowClock = std::chrono::steady_clock::now();
        auto betweenClock = std::chrono::duration_cast<std::chrono::microseconds>(CorePipeline::updateNowClock - CorePipeline::updatePrevClock);
        CorePipeline::updatePrevClock = CorePipeline::updateNowClock;
        auto tempDeltatime = (((float)betweenClock.count()) / 1000);
        CorePipeline::totalTime += tempDeltatime;

        if (CorePipeline::totalTime >= CorePipeline::targetFrameBetween || (!CorePipeline::targetFrameLock))
        {
            glutMainLoopEvent();
            FrameUpdate();

            CorePipeline::deltaTime = CorePipeline::totalTime / 1000.0f;

            world->WorldUpdate();
            animator->Update(CorePipeline::deltaTime);
            //world->mainCameraObject.lock()->transform.lock()->rotation = glm::vec3(angleY * 1 * D2R, angleX * 1 * D2R, 0);

            glutPostRedisplay();

            if (CorePipeline::targetFrameLock)
                while (CorePipeline::totalTime >= CorePipeline::targetFrameBetween)
                    CorePipeline::totalTime = CorePipeline::totalTime - CorePipeline::targetFrameBetween;
            else
                CorePipeline::totalTime = 0;
        }
        else std::this_thread::sleep_for(std::chrono::microseconds(10));
    }


    ImGUIClose();

    return 0;
}

GLvoid drawScene()
{
    ImGUIUpdate();
    //glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

    

    glEnable(GL_DEPTH_TEST);
    testShader.Bind();

    glEnable(GL_CULL_FACE);
    //glFrontFace(GL_BACK);

    /*
    auto location = glGetUniformLocation(testMaterial->shader->shaderID, "matM");
    if (location != -1)
    {
        auto modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::scale(modelMatrix, glm::vec3((float)windowY / windowX, 1, 1));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -1, 0));
        modelMatrix = glm::rotate(modelMatrix, angle * D2R, glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1));
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }
    location = glGetUniformLocation(testShader.shaderID, "u_color");
    if (location != -1)
    {
        auto modelcolor = color;
        glUniform4f(location, modelcolor.x, modelcolor.y, modelcolor.z, modelcolor.w);
    }

    */

    //testMaterial->Render(testModel)->IndexRender(testModel);
    //Material(&testShader).Render(testModel)->ArrayRender(testModel);
    //testRenderData->RenderingArray();
    /*
    for (int i = 0; i < klee.size(); i++)
    {
        klee[i]->material->shader->Bind();
        klee[i]->material->shader->uniformStorage->UpdateUniforms(*world->uniformStorage.get());
        klee[i]->material->shader->uniformStorage->UpdateUniforms(*klee[i]->material->uniformStorage.get());
        for (int j = 0; j < klee[i]->material->shader->uniformStorage->uniformList.size(); j++)
            klee[i]->material->shader->uniformStorage->uniformList[j].Bind();
        klee[i]->RenderingIndex();
    }
    */
    //testRenderData->RenderingIndex();

    animShader.Bind();
    auto transforms = animator->GetFinalBoneMatrices();
    for (int i = 0; i < transforms.size(); ++i)
        animShader.uniformStorage->PushUniform(UniformSegment(
                ("finalBonesMatrices[" + std::to_string(i) + "]").c_str(),
                GL_FLOAT_MAT4).SetData(transforms[i]));

        //ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    world->WorldRender();
    ImGUIRender();

    glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
    ImGui_ImplGLUT_ReshapeFunc(w, h);
    CorePipeline::screenW = w;
    CorePipeline::screenH = h;
    glViewport(0, 0, (int)CorePipeline::screenW, (int)CorePipeline::screenH);
}

void Update(int updateID)
{

    //glutPostRedisplay();
    //glutTimerFunc(updateID, Update, 16);
}

void KeyboardDown(unsigned char key, int x, int y) { ImGui_ImplGLUT_KeyboardFunc(key, x, y); Keyboard((int)key, false, GLUT_DOWN, x, y); }
void KeyboardUp(unsigned char key, int x, int y) { ImGui_ImplGLUT_KeyboardUpFunc(key, x, y); Keyboard((int)key, false, GLUT_UP, x, y); }
void KeyboardSpecDown(int key, int x, int y) { ImGui_ImplGLUT_SpecialFunc(key, x, y); Keyboard(key + 0xffff, true, GLUT_DOWN, x, y); }
void KeyboardSpecUp(int key, int x, int y) { ImGui_ImplGLUT_SpecialUpFunc(key, x, y); Keyboard(key + 0xffff, true, GLUT_UP, x, y); }
void Keyboard(int key, bool spec, int state, int x, int y)
{
    char specKey = 0;
    if (spec)
        specKey = key - 0xffff;

    switch (key) {
    case 'q':
        //keyUp = state == GLUT_DOWN ? 1 : 0;
        if (state == GLUT_DOWN)
        {
            animator->SetAnimation(model->animationList[0]);
            animator->Reset();
            animator->Play();
        }
        break;
    case 'w':
        //keyUp = state == GLUT_DOWN ? 1 : 0;
        if (state == GLUT_DOWN)
        {
            animator->SetAnimation(model->animationList[1]);
            animator->Reset();
            animator->Play();
        }
        break;
    case 'e':
        //keyUp = state == GLUT_DOWN ? 1 : 0;
        if (state == GLUT_DOWN)
        {
            animator->SetAnimation(model->animationList[4]);
            animator->Reset();
            animator->Play();
        }
        break;
    case 'r':
        //keyUp = state == GLUT_DOWN ? 1 : 0;
        if (state == GLUT_DOWN)
        {
            animator->SetAnimation(model->animationList[5]);
            animator->Reset();
            animator->Play();
        }
        break;
    }
    
    switch (specKey)
    {
    case GLUT_KEY_LEFT:
        break;
    case GLUT_KEY_RIGHT:
        break;
    default:
        break;
    }
}

bool mouseLeftPush = false;
void Mouse(int button, int state, int x, int y)
{
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
    glm::vec2 mousePos = glm::vec2(((float)x / CorePipeline::screenW) * 2 - 1, (((float)y / CorePipeline::screenH) * 2 - 1) * -1);
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouseLeftPush = true;
        mouseX = mousePos.x;
        mouseY = mousePos.y;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        mouseLeftPush = false;
    }
}

void Motion(int x, int y)
{
    ImGui_ImplGLUT_MotionFunc(x, y);
    glm::vec2 mousePos = glm::vec2(((float)x / CorePipeline::screenW) * 2 - 1, (((float)y / CorePipeline::screenH) * 2 - 1) * -1);
    if (mouseLeftPush)
    {
        angleX += (mousePos.x - mouseX) * 100;
        angleY += (mousePos.y - mouseY) * 100;
        mouseX = mousePos.x;
        mouseY = mousePos.y;
    }
}

void MouseWheel(int button, int dir, int x, int y)
{
    ImGui_ImplGLUT_MouseWheelFunc(button, dir, x, y);
}

/*
http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
OpenGL 좋은 튜토리얼 자료..

http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
*/