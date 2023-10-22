// -------------
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>

#include <fstream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
// -------------

#include <random>
#include <math.h>



#pragma region GL_Shader

// glew32.lib freeglut.lib assimp-vc143-mt.lib

#define D2R 0.01745329251994327f
#define R2D 57.2957795130823799f

#define Max(a, b) (a>b?a:b)
#define Min(a, b) (a<b?a:b)


void splitLine()
{
    std::cerr << "──────────────────────────────────────────────────────────────────────────────────────────\n";
}

char* filetobuf(const char* file)
{
    std::ifstream in(file, std::ios_base::binary);
    if (!in)
    {
        std::cerr << std::setw(10) << "ERROR: " << file << "Shader File Load - Falled : 경로 잘못됨" << std::endl;
        return nullptr;
    }
    in.seekg(0, std::ios_base::end);
    long len = in.tellg();
    char* buf = new char[len + 1];
    in.seekg(0, std::ios_base::beg);
    int cnt = -1;
    while (in >> std::noskipws >> buf[++cnt]) {}
    buf[len] = 0;
    return buf;
}

bool randomInit = false;
std::default_random_engine engine;
int RandomInt()
{
    if (randomInit == false)
    {
        randomInit = true;
        engine.seed(1234);
    }
    return engine();
}


struct hash_str {
    size_t operator()(const char* s) const {
        size_t v = 0;
        while (char c = *s++) {
            v = (v << 6) + (v << 16) - v + c;
            return std::hash<int>()(v);
        }
    }
};
struct equal_str {
    bool operator () (const char* a, const char* b) const {
        return strcmp(a, b) == 0;
    }
};



class ShaderCode;
class Shader;
class ShaderAttribute;
class ModelInfo2;
class ModelInfo;
class Material
{
public:
    Shader* shader;
    Material(Shader* shader);
    void SetShader(Shader* shader);
    unsigned int drawType = GL_TRIANGLES;
    void IndexRender(ModelInfo* model);
    void ArrayRender(ModelInfo* model);
    Material* Render(ModelInfo* model);
};
class RenderData;
class AttributeSegment;
class ModelBufferData;
class MeshVertex
{
public:
    glm::vec3 pos = glm::vec3(1,1,1);
    glm::vec3 normal = glm::vec3(1, 1, 1);
    glm::vec3 color = glm::vec3(1, 1, 1);
    glm::vec3 tangent = glm::vec3(1, 1, 1);
    glm::vec2 uv0 = glm::vec2(1, 1);
    glm::vec2 uv1 = glm::vec2(1, 1);
    glm::vec2 uv2 = glm::vec2(1, 1);
    glm::vec2 uv3 = glm::vec2(1, 1);
};

class MeshTexture
{
public:
    unsigned int id;
    std::string type;
};

class Mesh
{
public:
    std::vector<MeshVertex> vertexs;
    std::vector<unsigned int> indexs;
    std::vector<MeshTexture> textures;
    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        //mesh->
    }
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
        std::cout << std::setw(10) << "Try: " << "Shader Code Delete - Memory Clear Path : " << path << "\n";
    }
};

class ShaderAttribute
{
public:
    ShaderAttribute(const char* name, int location, int col, int row, unsigned int rawType, unsigned int type, unsigned int normalized)
    {
        this->name = name;
        this->location = location;
        this->col = col;
        this->row = row;
        this->rawType = rawType;
        this->type = type;
        this->normalized = normalized;
    }

    const char* name = nullptr;
    int location = 1;
    int col = 1;
    int row = 1;
    unsigned int rawType = GL_FLOAT;
    unsigned int type = GL_FLOAT;
    unsigned int normalized = GL_FALSE;
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

    std::vector<ShaderAttribute> attributeInfos;

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

            int count = 0;
            glGetProgramiv(this->shaderID, GL_ACTIVE_ATTRIBUTES, &count);
            if (count == 0)
            {
                std::cout << std::setw(10) << "Info: " << "  Attribute: 해당 Shader는 아무 Attribute도 없음." << std::endl;
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
                std::cout << std::setw(10) << "Info: " << "  Attribute: name:" << name << ", nameLength:" << length << ", size:" << size << ", type:" << type << "\n";
                int elementType = GL_FLOAT;
                int col = 1;
                int row = size;
                switch (type)
                {
                case GL_FLOAT: elementType = GL_FLOAT; col = 1; row = 1; break;
                case GL_FLOAT_VEC2: elementType = GL_FLOAT; col = 2; row = 1; break;
                case GL_FLOAT_VEC3: elementType = GL_FLOAT; col = 3; row = 1; break;
                case GL_FLOAT_VEC4: elementType = GL_FLOAT; col = 4; row = 1; break;
                case GL_FLOAT_MAT2: elementType = GL_FLOAT; col = 2; row = 2; break;
                case GL_FLOAT_MAT3: elementType = GL_FLOAT; col = 3; row = 3; break;
                case GL_FLOAT_MAT4: elementType = GL_FLOAT; col = 4; row = 4; break;
                case GL_FLOAT_MAT2x3: elementType = GL_FLOAT; col = 2; row = 3; break;
                case GL_FLOAT_MAT2x4: elementType = GL_FLOAT; col = 2; row = 4; break;
                case GL_FLOAT_MAT3x2: elementType = GL_FLOAT; col = 3; row = 2; break;
                case GL_FLOAT_MAT3x4: elementType = GL_FLOAT; col = 3; row = 4; break;
                case GL_FLOAT_MAT4x2: elementType = GL_FLOAT; col = 4; row = 2; break;
                case GL_FLOAT_MAT4x3: elementType = GL_FLOAT; col = 4; row = 3; break;

                case GL_DOUBLE: elementType = GL_DOUBLE; col = 1; row = 1; break;
                case GL_DOUBLE_VEC2: elementType = GL_DOUBLE; col = 2; row = 1; break;
                case GL_DOUBLE_VEC3: elementType = GL_DOUBLE; col = 3; row = 1; break;
                case GL_DOUBLE_VEC4: elementType = GL_DOUBLE; col = 4; row = 1; break;
                case GL_DOUBLE_MAT2: elementType = GL_DOUBLE; col = 2; row = 2; break;
                case GL_DOUBLE_MAT3: elementType = GL_DOUBLE; col = 3; row = 3; break;
                case GL_DOUBLE_MAT4: elementType = GL_DOUBLE; col = 4; row = 4; break;
                case GL_DOUBLE_MAT2x3: elementType = GL_DOUBLE; col = 2; row = 3; break;
                case GL_DOUBLE_MAT2x4: elementType = GL_DOUBLE; col = 2; row = 4; break;
                case GL_DOUBLE_MAT3x2: elementType = GL_DOUBLE; col = 3; row = 2; break;
                case GL_DOUBLE_MAT3x4: elementType = GL_DOUBLE; col = 3; row = 4; break;
                case GL_DOUBLE_MAT4x2: elementType = GL_DOUBLE; col = 4; row = 2; break;
                case GL_DOUBLE_MAT4x3: elementType = GL_DOUBLE; col = 4; row = 3; break;

                case GL_INT: elementType = GL_INT; col = 1; row = 1; break;
                case GL_INT_VEC2: elementType = GL_INT; col = 2; row = 1; break;
                case GL_INT_VEC3: elementType = GL_INT; col = 3; row = 1; break;
                case GL_INT_VEC4: elementType = GL_INT; col = 4; row = 1; break;

                case GL_UNSIGNED_INT: elementType = GL_UNSIGNED_INT; col = 1; row = 1; break;
                case GL_UNSIGNED_INT_VEC2: elementType = GL_UNSIGNED_INT; col = 2; row = 1; break;
                case GL_UNSIGNED_INT_VEC3: elementType = GL_UNSIGNED_INT; col = 3; row = 1; break;
                case GL_UNSIGNED_INT_VEC4: elementType = GL_UNSIGNED_INT; col = 4; row = 1; break;

                }
                std::cout << std::setw(10) << "Try: " << "  Auto Push Attribute Info\n";
                char* name2;
                std::string(name).copy(name2 = new char[length + 1](), length);
                SetAttribute(name2, col, row, type, elementType, GL_FLOAT);
                std::cout << "\n";
            }
            std::cout << "\n";
        }
    }
    void SetAttribute(const char* name, int col, int row, unsigned int rawType, unsigned int type, unsigned int normalized)
    {
        GLint positionAttribute = glGetAttribLocation(this->shaderID, name);
        if (positionAttribute == -1) {
            std::cout << std::setw(10) << "Warring: " << std::setw(22) << "  Failed SetAttribute" << " - Shader에 존재하지 않는 Attribute. name : " << name << "\n";
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
            std::cout << std::setw(10) << "Try: " << "  New Add SetAttribute" << " - name : " << name << "\n";
            attributeInfos.push_back(ShaderAttribute{ name, positionAttribute, col, row, rawType, type, normalized });
        }
        else
        {
            std::cout << std::setw(10) << "Try: " << "  Update SetAttribute" << " - name : " << name << "\n";
            attributeInfos[findIndex] = ShaderAttribute{ name, positionAttribute, col, row, rawType, type, normalized };
        }
    }
    static void* GL_TypeToNewArray(unsigned int type, int size)
    {
        switch (type)
        {
        case GL_FLOAT: return new float[size]();
        case GL_DOUBLE: return new double[size]();
        case GL_INT: return new int[size]();
        case GL_BYTE: return new char[size]();
        case GL_SHORT: return new short[size]();
        case GL_UNSIGNED_INT: return new unsigned int[size]();
        case GL_UNSIGNED_BYTE: return new unsigned char[size]();
        case GL_UNSIGNED_SHORT: return new unsigned short[size]();
        case GL_BOOL: return new bool[size]();
        }
        return nullptr;
    }
    static int GL_TypeToSizeOf(unsigned int type)
    {
        switch (type)
        {
        case GL_FLOAT: return sizeof(float) * 1;
        case GL_FLOAT_VEC2: return sizeof(float) * 2;
        case GL_FLOAT_VEC3: return sizeof(float) * 3;
        case GL_FLOAT_VEC4: return sizeof(float) * 4;
        case GL_FLOAT_MAT2: return sizeof(float) * 4;
        case GL_FLOAT_MAT3: return sizeof(float) * 9;
        case GL_FLOAT_MAT4: return sizeof(float) * 16;
        case GL_FLOAT_MAT2x3: return sizeof(float) * 6;
        case GL_FLOAT_MAT2x4: return sizeof(float) * 8;
        case GL_FLOAT_MAT3x2: return sizeof(float) * 6;
        case GL_FLOAT_MAT3x4: return sizeof(float) * 12;
        case GL_FLOAT_MAT4x2: return sizeof(float) * 8;
        case GL_FLOAT_MAT4x3: return sizeof(float) * 12;

        case GL_DOUBLE: return sizeof(double) * 1;
        case GL_DOUBLE_VEC2: return sizeof(double) * 2;
        case GL_DOUBLE_VEC3: return sizeof(double) * 3;
        case GL_DOUBLE_VEC4: return sizeof(double) * 4;
        case GL_DOUBLE_MAT2: return sizeof(double) * 4;
        case GL_DOUBLE_MAT3: return sizeof(double) * 9;
        case GL_DOUBLE_MAT4: return sizeof(double) * 16;
        case GL_DOUBLE_MAT2x3: return sizeof(double) * 6;
        case GL_DOUBLE_MAT2x4: return sizeof(double) * 8;
        case GL_DOUBLE_MAT3x2: return sizeof(double) * 6;
        case GL_DOUBLE_MAT3x4: return sizeof(double) * 12;
        case GL_DOUBLE_MAT4x2: return sizeof(double) * 8;
        case GL_DOUBLE_MAT4x3: return sizeof(double) * 12;

        case GL_INT: return sizeof(int) * 1;
        case GL_INT_VEC2: return sizeof(int) * 1;
        case GL_INT_VEC3: return sizeof(int) * 1;
        case GL_INT_VEC4: return sizeof(int) * 1;

        case GL_UNSIGNED_INT: return sizeof(unsigned int) * 1;
        case GL_UNSIGNED_INT_VEC2: return sizeof(unsigned int) * 1;
        case GL_UNSIGNED_INT_VEC3: return sizeof(unsigned int) * 1;
        case GL_UNSIGNED_INT_VEC4: return sizeof(unsigned int) * 1;

        }
    }
};
unsigned int Shader::currentShaderID = UINT_MAX;
unsigned int Shader::currentVAOID = 0;
std::vector<const char*> Shader::indexAttributeNameList = std::vector<const char*>({"vertexIndex", "vertexColorIndex", "normalIndex", "uv0Index" });
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
    unsigned int type = GL_FLOAT;
    unsigned int typeSize = -1;

    int location = -1;
    int col = 0;
    int row = 0;
    unsigned int normalized = GL_FALSE;

    AttributeSegment(const char* name, int segmentCount)
    {
        int nameLength = strlen(name);
        strncpy((this->name = new char[nameLength + 1]()), name, nameLength);
        this->segmentCount = segmentCount;
        
        this->rawBuffer;
        this->offsetCount = -1;
        this->blockCount = -1;
        this->type = GL_FLOAT;
        this->typeSize = -1;
    }
    AttributeSegment(const AttributeSegment& origin)
    {
        int nameLength = strlen(origin.name);
        strncpy((this->name = new char[nameLength + 1]()), origin.name, nameLength);
        this->segmentCount = origin.segmentCount;

        this->rawBuffer = origin.rawBuffer;
        this->offsetCount = origin.offsetCount;
        this->blockCount = origin.blockCount;
        this->type = origin.type;
        this->typeSize = origin.typeSize;
        this->location = origin.location;
        this->col = origin.col;
        this->row = origin.row;
        this->normalized = origin.normalized;
    }
    AttributeSegment(AttributeSegment&& other)
    {
        this->name = other.name;
        this->segmentCount = other.segmentCount;

        this->rawBuffer = other.rawBuffer;
        this->offsetCount = other.offsetCount;
        this->blockCount = other.blockCount;
        this->type = other.type;
        this->typeSize = other.typeSize;
        this->location = other.location;
        this->col = other.col;
        this->row = other.row;
        this->normalized = other.normalized;
        other.name = nullptr;
    }
    AttributeSegment& operator =(const AttributeSegment& other)
    {
        int nameLength = strlen(other.name);
        strncpy((this->name = new char[nameLength + 1]()), other.name, nameLength);
        this->segmentCount = other.segmentCount;

        this->rawBuffer = other.rawBuffer;
        this->offsetCount = other.offsetCount;
        this->blockCount = other.blockCount;
        this->type = other.type;
        this->typeSize = other.typeSize;
        this->location = other.location;
        this->col = other.col;
        this->row = other.row;
        this->normalized = other.normalized;
        return *this;
    }
    AttributeSegment& operator =(AttributeSegment && other)
    {
        this->name = other.name;
        this->segmentCount = other.segmentCount;

        this->rawBuffer = other.rawBuffer;
        this->offsetCount = other.offsetCount;
        this->blockCount = other.blockCount;
        this->type = other.type;
        this->typeSize = other.typeSize;
        this->location = other.location;
        this->col = other.col;
        this->row = other.row;
        this->normalized = other.normalized;
        other.name = nullptr;
        return *this;
    }
    
    AttributeSegment& ConvertRealSegment(std::shared_ptr<void> rawBuffer, unsigned int type, int offsetCount, int blockCount)
    {
        this->rawBuffer = rawBuffer;
        this->type = (int)type;
        this->typeSize = Shader::GL_TypeToSizeOf(type);
        this->offsetCount = offsetCount;
        this->blockCount = blockCount;
        return *this;
    }
    void Destroy()
    {
        delete[] (this->name);
        segmentCount = -1;
    }
    ~AttributeSegment()
    {
        this->Destroy();
    }
};
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
        //delete[] rawBuffer
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
        for (int i = resultBuferDatas.size()-1; i >= 0; i--) //역방향
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
                for (int k = 0; k < resultBuferDatas[i].attributeInfos.size(); k++)
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
    //std::vector<> 

    Material* material = nullptr;
    ModelInfo2* modelData = nullptr;

    bool activeIndexRender = false;

    RenderData(Material* material, ModelInfo2* modelData)
    {
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

    void RenderingArray()
    {
        this->material->shader->Bind();
        BindVAO();
        int vertexCount = renderBufferList[0].size / renderBufferList[0].GetBlockCount();
        if(vertexCount % 3 != 0)
            std::cout << std::setw(10) << "Warring: " << "ArrayRender의 Count가 3의 배수가 아닙니다."  << "\n";
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
            std::cerr << std::setw(10) << "ERROR: " << "Index Buffer가 존재하지 않는 Model을 Index 렌더링\n " << std::endl;
        }
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
                std::cout << std::setw(10) << "Try: " << "Material -> Shader Connect  shaderID : " << shader << "\n";
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
                    std::cerr << std::setw(10) << "ERROR: " << "Material -> Shader Connected Failed : 쉐이더에 필요한 attribute를 찾을 수 없습니다. name : " << nowAttriInfo.name << "\n";
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
        if(EBO == UINT32_MAX)
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


Material::Material(Shader* shader)
{
    SetShader(shader);
}
void Material::SetShader(Shader * shader)
{
    this->shader = shader;
}

unsigned int drawType = GL_TRIANGLES;
void Material::IndexRender(ModelInfo* model)
{
    glDrawElements(drawType, model->indexCount, GL_UNSIGNED_INT, 0);
}

void Material::ArrayRender(ModelInfo* model)
{
    glDrawArrays(drawType, 0, model->arrayCount);
}

Material* Material::Render(ModelInfo* model)
{
    this->SetShader(model->SetShaderConnect(shader));
    this->shader->Bind();
    return this;
}


ShaderCode LoadShader(const char* path)
{
    ShaderCode shaderCode;
    shaderCode.path = path;
    shaderCode.code = filetobuf(path);
    shaderCode.isShaderLoad = false;
    if (shaderCode.code != nullptr)
        shaderCode.isShaderLoad = true;
    std::cout << std::setw(10) << "Try: " << "Shader Load Success - Path : \"" << path << "\"\n";
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
        std::cerr << std::setw(10) << "ERROR: " << "Shader 컴파일 실패,  Type : " << ((shader.shaderType == GL_VERTEX_SHADER) ? "Vertex Shader" : "Fragment Shader") << "\n"<< errorLog << std::endl;
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
    std::cout << std::setw(10) << "Try: " << "Shader Compile Success - ShaderCodeID : " << shader.shaderCodeID << ", Type:" << ((shader.shaderType == GL_VERTEX_SHADER) ? "Vertex Shader" : "Fragment Shader") << "\n";
    return std::move(shader);
}

Shader CreateShaderProgram(ShaderCode&& vertexShaderCode, ShaderCode&& fragmentShaderCode)
{
    if (!(vertexShaderCode.isShaderLoad && vertexShaderCode.isShaderCompiled))
    {
        std::cerr << std::setw(10) << "ERROR: " << "Program 생성 실패 : 잘못된 VertexShader" << (vertexShaderCode.isShaderLoad?" - Load 되지 않은 Shader":"") << (vertexShaderCode.isShaderCompiled ? " - Compile 실패한 Shader" : "") <<"\n";
        std::cout << std::setw(10) << "Try: " << "Shader Compile 실패 -> Error Shader 반환" << "\n";
        vertexShaderCode.Delete();
        fragmentShaderCode.Delete();
        return errorShader;
    }
    if (!(vertexShaderCode.isShaderLoad && vertexShaderCode.isShaderCompiled))
    {
        std::cerr << std::setw(10) << "ERROR: " << "Program 생성 실패 : 잘못된 FragmentShader" << (fragmentShaderCode.isShaderLoad ? " - Load 되지 않은 Shader" : "") << (fragmentShaderCode.isShaderCompiled ? " - Compile 실패한 Shader" : "") << "\n";
        std::cout << std::setw(10) << "Try: " << "Shader Compile 실패 -> Error Shader 반환" << "\n";
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
        std::cerr << std::setw(10) << "ERROR: " << "Program 연결 실패\n " << errorLog << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        std::cout << std::setw(10) << "Try: " << "Shader Compile 실패 -> Error Shader 반환" << "\n";
        vertexShaderCode.Delete();
        fragmentShaderCode.Delete();
        newShader.Delete();
        return errorShader;
    }

    std::cout << std::setw(10) << "Try: " << "Shader Program Create Completed - ShaderProgramCode : " << newShader.shaderID << ",  Path : \"" << vertexShaderCode.path << "\" / \"" << fragmentShaderCode.path << "\"\n";
    vertexShaderCode.Delete();
    fragmentShaderCode.Delete();

    newShader.isCompleted = true;
    newShader.vertexCode = vertexShaderCode;
    newShader.fragmentCode = fragmentShaderCode;
    newShader.Init();
    newShader.Bind();
    return newShader;
}

#pragma endregion



class Transform
{
public:
    Transform* parent = nullptr;
    std::vector<Transform*> childs;

    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 rotation = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(0, 0, 0);

    glm::mat4 GetModelToWorldAll()
    {
        auto tempNow = this;
        while (tempNow != nullptr)
        {
            auto localTRS = glm::mat4(1);
            localTRS = glm::translate(localTRS, tempNow->position);
            localTRS = glm::rotate(localTRS, tempNow->rotation.y, glm::vec3(0, 1, 0));
            localTRS = glm::rotate(localTRS, tempNow->rotation.x, glm::vec3(1, 0, 0));
            localTRS = glm::rotate(localTRS, tempNow->rotation.z, glm::vec3(0, 0, 1));
            localTRS = glm::scale(localTRS, tempNow->scale);
            tempNow = tempNow->parent;
        }
    }
    void SetParent(Transform* parent)
    {
        if (this->parent != nullptr)
        {
            for (int i = 0; i < this->parent->childs.size(); i++)
            {
                if (this->parent->childs[i] == this)
                {
                    this->parent->childs.erase(this->parent->childs.begin() + i);
                    i--;
                }
            }
        }
        this->parent = parent;
        this->parent->childs.push_back(this);
    }
    void AddChild(Transform* child)
    {
        child->SetParent(this);
    }
};


int windowX = 1280;
int windowY = 720;


GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Update(int updateID);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void KeyboardSpec(int key, int x, int y);

//----new system----
Shader testShader;
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

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 { //--- 윈도우 생성하기
{
    std::cout.setf(std::ios::right);
    glutInit(&argc, argv); // glut 초기화
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
    glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
    glutInitWindowSize(windowX, windowY); // 윈도우의 크기 지정
    glutCreateWindow("GLEW_1"); // 윈도우 생성

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) // glew 초기화
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Init Completed\n";
    splitLine();

    errorShader = CreateShaderProgram(
        CompileShader(LoadShader("gl_Error_Vert.glsl"), GL_VERTEX_SHADER),
        CompileShader(LoadShader("gl_Error_Frag.glsl"), GL_FRAGMENT_SHADER));
    //errorShader.SetAttribute("positionOS", 4, 1, GL_FLOAT_VEC4, GL_FLOAT, GL_FALSE);
    splitLine();

    testShader = CreateShaderProgram(
        CompileShader(LoadShader("gl_vertex.glsl"), GL_VERTEX_SHADER),
        CompileShader(LoadShader("gl_fragment.glsl"), GL_FRAGMENT_SHADER));
    //testShader.SetAttribute("positionOS", 4, 1, GL_FLOAT_VEC4, GL_FLOAT, GL_FALSE);
    //testShader.SetAttribute("vertexColor", 4, 1, GL_FLOAT_VEC4, GL_FLOAT, GL_FALSE);
    splitLine();


    testMaterial = new Material(&testShader);
    testMaterial->drawType = GL_TRIANGLES;


    /* 방법 1.
    float testPosition[16] = { 0, 0, 0, 1,
               0.5, 0, 0, 1,
               0.5, -0.5, 0, 1,
               0, -0.5, 0, 1 };
    float testVertexColor[16] = { 0, 1, 0, 1,s
                1, 1, 0, 1,
                1, 0, 0, 1,
                0, 0, 0, 1 };
    unsigned int testIndexs[6] = { 0, 3, 2,
                            0, 2, 1 };
    testModel = new ModelInfo();
    testModel->Init();
    testModel->SetIndex(testIndexs, sizeof(testIndexs));
    testModel->SetBufferData("positionOS", testPosition, sizeof(testPosition), 4 * sizeof(float), 0);
    testModel->SetBufferData("vertexColor", testVertexColor, sizeof(testVertexColor), 4 * sizeof(float), 0);
    */
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
    
    std::cout << "Load Mesh : " << pScene->mNumMeshes << "\n";
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

    
    testModel = new ModelInfo();
    testModel->Init();
    testModel->SetIndexBuffer(testIndexs, sizeof(testIndexs));
    testModel->SetArrayCount(3);
    testModel->SetBufferData("positionOS", testTotalVertexBuffer, sizeof(testTotalVertexBuffer), 8 * sizeof(float), 0); // todo : 이부분 다시 체크 3* sizeof(float)로
    testModel->SetBufferData("vertexColor", testTotalVertexBuffer, sizeof(testTotalVertexBuffer), 8 * sizeof(float), 4 * sizeof(float));

    
    float* meshVertexBuffer;
    unsigned int* meshIndexBuffer;
    std::vector<unsigned int> meshIndexList;
    std::vector<float> meshVertexList;

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
            { std::shared_ptr<void>(meshVertexBuffer), (int)(meshVertexList.size()), GL_FLOAT, {{"positionOS", 4}, {"vertexColor", 4} } },
            { std::shared_ptr<void>(meshIndexBuffer), (int)(meshIndexList.size()), GL_UNSIGNED_INT, {{"vertexIndex", 1}}}
            });
        klee.push_back(new RenderData(testMaterial, m));
        meshIndexList.clear();
        meshVertexList.clear();
    }

    auto vertexSize = 8 * 4 * 2;
    auto indexSize = 12 * 3;
    float* vertexBuffer = new float[vertexSize];
    auto indexBuffer = new unsigned int[indexSize];
    int index;
    std::vector<glm::vec4> vertexList;
    std::vector<glm::vec4> colorList;

    //testModel = new ModelInfo();
    //testModel->Init();
    vertexList.push_back(glm::vec4(-0.5, -0.5, 0.5, 1));
    vertexList.push_back(glm::vec4(0.5, -0.5, 0.5, 1));
    vertexList.push_back(glm::vec4(-0.5, -0.5, -0.5, 1));
    vertexList.push_back(glm::vec4(0.5, -0.5, -0.5, 1));
    vertexList.push_back(glm::vec4(-0.5, 0.5, 0.5, 1));
    vertexList.push_back(glm::vec4(0.5, 0.5, 0.5, 1));
    vertexList.push_back(glm::vec4(-0.5, 0.5, -0.5, 1));
    vertexList.push_back(glm::vec4(0.5, 0.5, -0.5, 1));

    colorList.push_back(glm::vec4(1, 0, 0, 1));
    colorList.push_back(glm::vec4(0, 1, 0, 1));
    colorList.push_back(glm::vec4(0, 0, 1, 1));
    colorList.push_back(glm::vec4(0, 1, 1, 1));
    colorList.push_back(glm::vec4(1, 1, 0, 1));
    colorList.push_back(glm::vec4(0, 0, 0, 1));
    colorList.push_back(glm::vec4(1, 1, 1, 1));
    colorList.push_back(glm::vec4(1, 0, 1, 1));

    index = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
            vertexBuffer[index++] = vertexList[i][j];
        for (int j = 0; j < 4; j++)
            vertexBuffer[index++] = colorList[i][j];
    }

    index = 0;
    indexBuffer[index++] = 0;
    indexBuffer[index++] = 1;
    indexBuffer[index++] = 2;
    indexBuffer[index++] = 2;
    indexBuffer[index++] = 1;
    indexBuffer[index++] = 3;

    indexBuffer[index++] = 5;
    indexBuffer[index++] = 7;
    indexBuffer[index++] = 1;
    indexBuffer[index++] = 7;
    indexBuffer[index++] = 3;
    indexBuffer[index++] = 1;

    indexBuffer[index++] = 4;
    indexBuffer[index++] = 5;
    indexBuffer[index++] = 0;
    indexBuffer[index++] = 5;
    indexBuffer[index++] = 1;
    indexBuffer[index++] = 0;

    indexBuffer[index++] = 6;
    indexBuffer[index++] = 4;
    indexBuffer[index++] = 0;
    indexBuffer[index++] = 6;
    indexBuffer[index++] = 0;
    indexBuffer[index++] = 2;

    indexBuffer[index++] = 6;
    indexBuffer[index++] = 2;
    indexBuffer[index++] = 7;
    indexBuffer[index++] = 7;
    indexBuffer[index++] = 2;
    indexBuffer[index++] = 3;

    indexBuffer[index++] = 4;
    indexBuffer[index++] = 6;
    indexBuffer[index++] = 5;
    indexBuffer[index++] = 5;
    indexBuffer[index++] = 6;
    indexBuffer[index++] = 7;

    ModelInfo2* m2 = new ModelInfo2({
            { std::shared_ptr<void>(vertexBuffer), vertexSize, GL_FLOAT, {{"positionOS", 4}, {"vertexColor", 4} } } ,
            { std::shared_ptr<void>(indexBuffer), indexSize, GL_UNSIGNED_INT, {{"vertexIndex", 1}} }
        });
    /*
    if (lines)
    {
        indexBuffer2 = new unsigned int[indexSize * 2];
        for (int i = 0; i < indexSize; i += 3)
        {
            indexBuffer2[i * 2 + 0] = indexBuffer[i + 0];
            indexBuffer2[i * 2 + 1] = indexBuffer[i + 1];

            indexBuffer2[i * 2 + 2] = indexBuffer[i + 1];
            indexBuffer2[i * 2 + 3] = indexBuffer[i + 2];

            indexBuffer2[i * 2 + 4] = indexBuffer[i + 2];
            indexBuffer2[i * 2 + 5] = indexBuffer[i + 0];
        }
        testModel->SetIndexBuffer(indexBuffer2, indexSize * 2 * sizeof(unsigned int));
        testMaterial->drawType = GL_LINES;
    }
    */
    
    //m->datas.push_back({ new float[8](), 8, GL_FLOAT, {{"positionOS", 4}} });
    /*
    auto clean = ModelInfo2::GetFilteringBufferDatas(m.datas);
    auto v = ModelInfo2::GetFilteringShaderBufferDatas(clean, &testShader);

    std::cout << "\n";
    std::cout << m.datas.size() << "\n";
    for (int i = 0; i < m.datas.size(); i++)
    {
        std::cout << "block\n";
        for (int j = 0; j < m.datas[i].attributeInfos.size(); j++)
            std::cout << m.datas[i].attributeInfos[j].name << ", " << &m.datas[i].attributeInfos[j].rawBuffer << ", " << m.datas[i].attributeInfos[j].type
            << ", " << m.datas[i].attributeInfos[j].typeSize << ", " << m.datas[i].attributeInfos[j].offsetCount << ", " << m.datas[i].attributeInfos[j].blockCount << "\n";
    }

    std::cout << "\n";
    std::cout << v.size()<<"\n";
    for (int i = 0; i < v.size(); i++)
    {
        std::cout << "block\n";
        for (int j = 0; j < v[i].attributeInfos.size(); j++)
            std::cout << v[i].attributeInfos[j].name << ", " << &v[i].attributeInfos[j].rawBuffer << ", " << v[i].attributeInfos[j].type
            << ", " << v[i].attributeInfos[j].typeSize << ", " << v[i].attributeInfos[j].offsetCount << ", " << v[i].attributeInfos[j].blockCount << "\n";
    }
    auto v2 = ModelInfo2::GetFilteringIndexBuffers(clean);
    std::cout << v2[0].attributeInfos[0].name << "\n";
    */

    testRenderData = new RenderData(testMaterial, m2);

    //testRenderData->indexBuffer = ModelInfo2::GetConvertPolyToWireIndexBuffer(testRenderData->indexBuffer);
    //testRenderData->renderBufferList = ModelInfo2::GetConvertIndexToArrayDataBuffers(testRenderData->indexBuffer, testRenderData->renderBufferList);
    //testRenderData->indexBuffer = ModelInfo2::GetConvertSequenceIndexBuffers(testRenderData->indexBuffer);
    testRenderData->UpdateAll();
    testMaterial->drawType = GL_TRIANGLES;

    glutDisplayFunc(drawScene); // 출력 함수의 지정
    glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(KeyboardSpec);

    glutTimerFunc(1, Update, 16);

    glutMainLoop(); // 이벤트 처리 시작


    return 0;
}
float mouseX = 0;
float angle = 0;
GLvoid drawScene()
{
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
    glEnable(GL_DEPTH_TEST);
    testShader.Bind();
    
    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_BACK);

    auto location = glGetUniformLocation(testMaterial->shader->shaderID, "matM");
    if (location != -1)
    {
        auto modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::scale(modelMatrix, glm::vec3((float)windowY/ windowX, 1, 1));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -1, 0));
        modelMatrix = glm::rotate(modelMatrix, angle * D2R, glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1));
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }
    /*
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
    for(int i = 0; i < klee.size(); i++)
        klee[i]->RenderingIndex();
    //testRenderData->RenderingIndex();
    glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
    glViewport(0, 0, w, h);
}

void Update(int updateID)
{

    glutPostRedisplay();
    glutTimerFunc(updateID, Update, 16);
}

void Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'a':
        break;
    }
}
void KeyboardSpec(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_LEFT:
        break;
    }
}

void Mouse(int button, int state, int x, int y)
{
    glm::vec2 mousePos = glm::vec2(((float)x / windowX) * 2 - 1, (((float)y / windowY) * 2 - 1) * -1);
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouseX = mousePos.x;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        
    }
}

void Motion(int x, int y)
{
    glm::vec2 mousePos = glm::vec2(((float)x / windowX) * 2 - 1, (((float)y / windowY) * 2 - 1) * -1);
    angle -= (mousePos.x - mouseX)*100;
    mouseX = mousePos.x;
}



/*
http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
OpenGL 좋은 튜토리얼 자료..

http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/

설계의 좋은 아이디어 떠오름.
데이터 매칭방식을 바꾼다.
 map 다 걷어냄.
Buffer을 집어넣을때
array주고
std::vector<Attribute>{로 넘기는 방식.}
이걸 만들면 Find함수도 만들어야함.
map 버그 많으니 안쓰는걸로..
전부 spliut 하는 대신, Buffer 단위로 크기 통합하는 처리방식..

그리고 ModelInfo랑 MeshData도 분리시키고
지금의 VAO는 Material이랑 묶는 하위 기능으로 분화.

개념적인 명확한 분리

Shader - 데이터 요청기.
Material - 데이터 주입기.
RenderData - 파편화된 데이터 묶음  Material을 토대로 자기가 주입해야할 데이터 파생.
Model - 모델 원시 데이터

예를 들어 2머티리얼,, 2Mesh
renderData = 머티리얼1 - model1
renderData = 머티리얼1 - model2
renderData = 머티리얼2 - model1
renderData = 머티리얼2 - model2
이렇게 파생구조를 형성해야
앞으로 올바른 파이프라인 구성이 가능할 것 같음..

SkinRenderer구성시 원시 model을 변형하고,, renderData들을 업데이트 시캬야함../


#파이썬은 class를 여러개의 함수들을 그룹핑 하는 용도로도 쓸 수 있다.

ModelData의 Index, Array형태
이걸 포멧팅하는 Render Data의 Index, Array 형태를 구분. 사실상 무조건 Array 형태겠지만 ㅇㅇ...
*/