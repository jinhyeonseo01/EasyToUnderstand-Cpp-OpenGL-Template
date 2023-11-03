// -------------
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>

#include <chrono>
#include <Thread>
#include <time.h>
#include <windows.h>

#include <fstream>

//imgui
#include "imgui/imgui.h"
#include "imgui_local/imgui_impl_glut.h"
#include "imgui_local/imgui_impl_opengl3.h"
//----------

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
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cerr << "──────────────────────────────────────────────────────────────────────────────────────────\n";
}
std::ostream& Log()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return std::cerr;
}
std::ostream& ErrorLog(std::ostream& o, const char* log = "Error", int depth = 0)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    for (int i = depth - 1; i >= 0; i--)
        o << ((i == 0) ? "       ├ " : "       │ ");
    o << std::setw(8);
    o << log;
    o << ": ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    return o;
}
std::ostream& NormalLog(std::ostream& o, const char* log = "Log", int depth = 0)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    for (int i = depth - 1; i >= 0; i--)
        o << ((i == 0) ? "       ├ " : "       │ ");
    o << std::setw(8);
    o << log;
    o << ": ";
    if (strcmp(log, "Try") == 0)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
    if (strcmp(log, "Info") == 0)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
    return o;
}
std::ostream& WarringLog(std::ostream& o, const char* log = "Warring", int depth = 0)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    for (int i = depth - 1; i >= 0; i--)
        o << ((i == 0) ? "       ├ " : "       │ ");
    o << std::setw(8);
    o << log;
    o << ": ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    return o;
}

char* filetobuf(const char* file)
{
    std::ifstream in(file, std::ios_base::binary);
    if (!in)
    {
        ErrorLog(std::cerr) << file << "Shader File Load - Falled : 경로 잘못됨\n";
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

int rand2()
{
    return RandomInt();
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
    /*
    AABB(glm::vec3 pos, glm::vec3 size)
    {
        this->min = pos - size;
        this->min = pos + size;

        this->min.x = max.x >= min.x ? min.x : max.x;
        this->max.x = max.x < min.x ? min.x : max.x;

        this->min.y = max.y >= min.y ? min.y : max.y;
        this->max.y = max.y < min.y ? min.y : max.y;

        this->min.z = max.z >= min.z ? min.z : max.z;
        this->max.z = max.z < min.z ? min.z : max.z;
    }
    AABB(glm::vec2 pos, glm::vec2 size)
    {
        this->min = glm::vec3(pos - size, 0);
        this->max = glm::vec3(pos + size, 0);

        this->min.x = max.x >= min.x ? min.x : max.x;
        this->max.x = max.x < min.x ? min.x : max.x;

        this->min.y = max.y >= min.y ? min.y : max.y;
        this->max.y = max.y < min.y ? min.y : max.y;

        this->min.z = max.z >= min.z ? min.z : max.z;
        this->max.z = max.z < min.z ? min.z : max.z;
    }
    */
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




class ShaderCode;
class Shader;
class ModelInfo2;
class ModelInfo;

class UniformSegment;
class AttributeSegment;
class UniformStorage;

class Material
{
public:
    Shader* shader;
    std::shared_ptr<UniformStorage> uniformStorage;

    Material(Shader* shader);
    void SetShader(Shader* shader);
    unsigned int drawType = GL_TRIANGLES;
    void IndexRender(ModelInfo* model);
    void ArrayRender(ModelInfo* model);
    Material* Render(ModelInfo* model);
};
class RenderData;
class ModelBufferData;

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

class MeshVertex
{
public:
    glm::vec3 pos = glm::vec3(1, 1, 1);
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
        NormalLog(std::cerr, "Try", 1) << "Shader Code Delete - Memory Clear Path : " << path << "\n";
    }
};
/*
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
*/
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
        
        case GL_TEXTURE_1D:         glUniform1i(location, data_vector_int.x); break;
        case GL_TEXTURE_1D_ARRAY:   glUniform1i(location, data_vector_int.x); break;
        
        case GL_TEXTURE_2D:         glUniform1i(location, data_vector_int.x); break;
        case GL_TEXTURE_2D_ARRAY:   glUniform1i(location, data_vector_int.x); break;

        case GL_TEXTURE_3D:         glUniform1i(location, data_vector_int.x); break;

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


Material::Material(Shader* shader)
{
    SetShader(shader);
    this->uniformStorage = std::shared_ptr<UniformStorage>(new UniformStorage());
}
void Material::SetShader(Shader* shader)
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

#pragma endregion

class GameObject;
class Component;
class Transform;
class World;
class CorePipeline;

class CorePipeline
{
public:
    static bool isGameExit;

    static bool targetFrameLock;
    static float targetFrame;
    static float targetFrameBetween;
    static float deltaTime;
    static float totalTime;
    static std::chrono::steady_clock::time_point updatePrevClock;
    static std::chrono::steady_clock::time_point updateNowClock;

    void GameExit()
    {
        glutLeaveMainLoop();
        isGameExit = true;
    }
};

bool CorePipeline::isGameExit = false;
bool CorePipeline::targetFrameLock = true;
float CorePipeline::targetFrame = 165;
float CorePipeline::targetFrameBetween = ((float)1000) / CorePipeline::targetFrame;
float CorePipeline::totalTime = 0;
float CorePipeline::deltaTime = 1 / CorePipeline::targetFrame;
std::chrono::steady_clock::time_point CorePipeline::updatePrevClock;
std::chrono::steady_clock::time_point CorePipeline::updateNowClock;

class World
{
public:
    std::vector<std::shared_ptr<GameObject>> gameObjectList;
    std::shared_ptr<UniformStorage> uniformStorage;

    World();

    std::weak_ptr<GameObject> CreateGameObject();
    bool AddGameObject(std::shared_ptr<GameObject> gameObject);
    void WorldUpdate();
    void WorldRender();
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
    bool activePrev = false;
    bool active = true;
    bool destroy = false;

    std::weak_ptr<GameObject> parent;
    std::vector<std::weak_ptr<GameObject>> childs;

    std::weak_ptr<Transform> transform;
    std::vector<std::shared_ptr<Component>> componentList;

    GameObject();
    template <class T>
    std::weak_ptr<T> AddComponent(T* component);
    template <class T>
    std::weak_ptr<T> GetComponent();
    template <class T>
    bool RemoveComponent(T* element);
    bool RemoveAtComponent(int index);
    template <class T>
    void DestroyComponent(T* element);
    void ClearComponent();
    
    bool SetParent(std::weak_ptr<GameObject> parent);
    void AddChild(GameObject* child);


    virtual void Create();
    virtual void Start();
    virtual void Update();
    virtual void LateUpdate();
    virtual void PostBehavior();
    virtual void Enable();
    virtual void Disable();
    virtual void BeforeRender();
};

//template<> bool GameObject::RemoveComponent(std::weak_ptr<Component> element);
//template<> std::weak_ptr<Component> GameObject::AddComponent(Component* element);
//template<> std::weak_ptr<Component> GameObject::GetComponent();
//template<> void GameObject::DestroyComponent(std::weak_ptr<Component> element);

class Component
{
public:
    bool activePrev = false;
    bool active = true;
    bool destroy = false;
    bool isFirstUpdate = true;
    std::weak_ptr<GameObject> gameObject;
    virtual void Create() {}
    virtual void Start() {  }
    virtual void Update() {  }
    virtual void LateUpdate() {  } //this->destroy = true;
    virtual void Destroy() {  }
    virtual void Enable() {  }
    virtual void Disable() {  }
    virtual void BeforeRender() {  }
};

World::World()
{
    uniformStorage = std::shared_ptr<UniformStorage>(new UniformStorage());
}

std::weak_ptr<GameObject> World::CreateGameObject()
{
    std::shared_ptr<GameObject> gameObject;
    this->AddGameObject(gameObject = std::shared_ptr<GameObject>(new GameObject()));
    gameObject->Create();
    return gameObject;
}

bool World::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
    for (int i = 0; i < gameObjectList.size(); i++)
        if (gameObjectList[i] == gameObject)
            return false;
    gameObjectList.push_back(gameObject);
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
    for (int i = 0; i < gameObjectList.size(); i++)
        gameObjectList[i]->BeforeRender();
}

class Transform : public Component, public std::enable_shared_from_this<Transform>
{
private:
    glm::vec3 positionPrev = glm::vec3(1234.5678f, 1234.5678f, 1234.5678f);
    glm::vec3 rotationPrev = glm::vec3(1234.5678f, 1234.5678f, 1234.5678f);
    glm::vec3 scalePrev = glm::vec3(1234.5678f, 1234.5678f, 1234.5678f);
    glm::mat4 TRSCache = glm::mat4(1);
public:
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 rotation = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(0, 0, 0);

    glm::mat4 GetModelToWorld()
    {
        if (positionPrev.x == position.x || positionPrev.y == position.y || positionPrev.z == position.z ||
            rotationPrev.x == rotation.x || rotationPrev.y == rotation.y || rotationPrev.z == rotation.z ||
            scalePrev.x == scale.x || scalePrev.y == scale.y || scalePrev.z == scale.z)
        {
            TRSCache = glm::mat4(1);
            TRSCache = glm::translate(TRSCache, this->position);
            TRSCache = glm::rotate(TRSCache, this->rotation.y, glm::vec3(0, 1, 0));
            TRSCache = glm::rotate(TRSCache, this->rotation.x, glm::vec3(1, 0, 0));
            TRSCache = glm::rotate(TRSCache, this->rotation.z, glm::vec3(0, 0, 1));
            TRSCache = glm::scale(TRSCache, this->scale);
        }
        return TRSCache;
    }
    glm::mat4 GetWorldToModel()
    {
        return glm::inverse(TRSCache);
    }

    glm::mat4&& GetModelToWorldAll()
    {
        auto tempNow = this;
        auto TotalTRS = glm::mat4(1);
        while (tempNow != nullptr)
        {
            if (!tempNow->gameObject.expired())
            {
                TotalTRS = TotalTRS * tempNow->GetModelToWorld();
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
    glm::mat4&& GetWorldToModelAll()
    {
        return glm::inverse(this->GetModelToWorldAll());
    }
};


GameObject::GameObject()
{
    active = true;
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

int windowX = 1280;
int windowY = 720;


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


int frameCount = 0;
float frameTotal = 0;
int frame[100];
std::chrono::steady_clock::time_point frameStartCheck;
std::chrono::steady_clock::time_point frameCheck[100];
void FrameInit(int count)
{
    frameCount = count;
    for (int i = 0; i < frameCount; i++)
    {
        frame[i] = -1;
        frameCheck[i] = std::chrono::steady_clock::now();
    }
    frameStartCheck = std::chrono::steady_clock::now();
}
void FrameUpdate()
{
    float tempFrameTotal = 0;
    for (int i = 0; i < frameCount; i++)
    {
        if (frame[i] != -1 || (frame[i] == -1 && std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - frameStartCheck).count() >= ((1000000.0f / frameCount) * i)))
        {
            auto now = std::chrono::steady_clock::now();
            if (frame[i] == -1)
                frameCheck[i] = now;
            if (std::chrono::duration_cast<std::chrono::microseconds>(now - frameCheck[i]).count() >= 1000000)
            {
                float weight = 1/((float)frameCount * 2.0f);
                frameTotal = (frameTotal * (1 - weight) + frame[i] * weight);
                frameCheck[i] = now;
                frame[i] = 0;
            }
            frame[i]++;
        }
    }
}


static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

World world;
float mouseX = 0;
float angle = 0;

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 { //--- 윈도우 생성하기
{
    SetWindowPos(GetConsoleWindow(), 0, 0, 0, 1500, 800, SWP_SHOWWINDOW);
    SetConsoleTitle(L"콘솔");
    std::cout.setf(std::ios::right);


    glutInit(&argc, argv); // glut 초기화
#ifdef __FREEGLUT_EXT_H__
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH); // 디스플레이 모드 설정
    glutInitWindowPosition(310, 180); // 윈도우의 위치 지정
    glutInitWindowSize(windowX, windowY); // 윈도우의 크기 지정
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


    
    
    timeBeginPeriod(1);
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    //----------------------------------------------------


    errorShader = CreateShaderProgram(
        CompileShader(LoadShader("./Shaders/gl_Error_Vert.glsl"), GL_VERTEX_SHADER),
        CompileShader(LoadShader("./Shaders/gl_Error_Frag.glsl"), GL_FRAGMENT_SHADER));
    //errorShader.SetAttribute("positionOS", 4, 1, GL_FLOAT_VEC4, GL_FLOAT, GL_FALSE);
    splitLine();

    testShader = CreateShaderProgram(
        CompileShader(LoadShader("./Shaders/gl_vertex.glsl"), GL_VERTEX_SHADER),
        CompileShader(LoadShader("./Shaders/gl_fragment.glsl"), GL_FRAGMENT_SHADER));
    //testShader.SetAttribute("positionOS", 4, 1, GL_FLOAT_VEC4, GL_FLOAT, GL_FALSE);
    //testShader.SetAttribute("vertexColor", 4, 1, GL_FLOAT_VEC4, GL_FLOAT, GL_FALSE);
    splitLine();

    /*
    uniform mat4 matM;
    uniform mat4 matrix_ModelToWorld; // model -> world
    uniform mat4 matrix_WorldToModel; // world -> model
    uniform mat4 matrix_ViewProjection; // world -> clip
    uniform mat4 matrix_View; // world -> view
    uniform mat4 matrix_Projection; // view -> clip
    */
    testMaterial = new Material(&testShader);
    testMaterial->drawType = GL_TRIANGLES;

    NormalLog(std::cout) << "테스트테스트2" << std::endl;
    NormalLog(std::cout) << "테스트테스트" << std::endl;
    NormalLog(std::cout, "Log", 1) << "테스트테스트" << std::endl;
    NormalLog(std::cout, "Log", 2) << "테스트테스트" << std::endl;
    NormalLog(std::cout, "ASDASD", 2) << "테스트테스트" << std::endl;
    NormalLog(std::cout, "Log", 3) << "테스트테스트" << std::endl;
    NormalLog(std::cout, "Log", 2) << "테스트테스트" << std::endl;

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

    std::shared_ptr<GameObject> A = world.CreateGameObject().lock();
    std::shared_ptr<GameObject> B = world.CreateGameObject().lock();
    A->SetParent(B);
    B->SetParent(A);
    //A->Update();
    //B->Update();
    //obj
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
    glutPassiveMotionFunc(Motion);
    glutMouseWheelFunc(MouseWheel);
    glutKeyboardFunc(KeyboardDown);
    glutKeyboardUpFunc(KeyboardUp);
    glutSpecialFunc(KeyboardSpecDown);
    glutSpecialUpFunc(KeyboardSpecUp);

    glutTimerFunc(1, Update, 16);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init();

    //ImGui_ImplGLUT_InstallFuncs();

    NormalLog(std::cerr, "Log") << "ImGUI 초기화 Completed\n";
    splitLine();

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

            world.WorldUpdate();
            world.WorldRender();
            auto modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
            modelMatrix = glm::rotate(modelMatrix, angle * D2R, glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1));
            world.uniformStorage->PushUniform(UniformSegment("matrix_ModelToWorld", GL_FLOAT_MAT4).SetData(modelMatrix));


            auto viewMatrix = glm::lookAt(glm::vec3(0,1,-2), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
            viewMatrix = glm::rotate(viewMatrix, 0.0f, glm::vec3(0, 0, 1));
            auto projectionMatrix = glm::perspective(60.0f * D2R, (float)windowX / windowY, 0.03f, 100.0f);

            world.uniformStorage->PushUniform(UniformSegment("matrix_ViewProjection", GL_FLOAT_MAT4).SetData(projectionMatrix * viewMatrix));

            glutPostRedisplay();

            if (CorePipeline::targetFrameLock)
                while (CorePipeline::totalTime >= CorePipeline::targetFrameBetween)
                    CorePipeline::totalTime = CorePipeline::totalTime - CorePipeline::targetFrameBetween;
            else
                CorePipeline::totalTime = 0;
        }
        else std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();

    //_CrtDumpMemoryLeaks();

    return 0;
}

GLvoid drawScene()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO();
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }
    ImGui::Render(); // IMGUI에 누적시키는 기능인가봄.

    //glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

    

    glEnable(GL_DEPTH_TEST);
    testShader.Bind();

    //glEnable(GL_CULL_FACE);
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
    */
    testMaterial->shader->uniformStorage->UpdateUniforms(*world.uniformStorage.get());

    for (int i = 0; i < testMaterial->shader->uniformStorage->uniformList.size(); i++)
        testMaterial->shader->uniformStorage->uniformList[i].Bind();
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
    for (int i = 0; i < klee.size(); i++)
        klee[i]->RenderingIndex();
    //testRenderData->RenderingIndex();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
    ImGui_ImplGLUT_ReshapeFunc(w, h);
    glViewport(0, 0, w, h);
    windowX = w;
    windowY = h;
    //glutPostRedisplay();
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
    case 'w':
        //keyUp = state == GLUT_DOWN ? 1 : 0;
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
    glm::vec2 mousePos = glm::vec2(((float)x / windowX) * 2 - 1, (((float)y / windowY) * 2 - 1) * -1);
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mouseLeftPush = true;
        mouseX = mousePos.x;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        mouseLeftPush = false;
    }
}

void Motion(int x, int y)
{
    ImGui_ImplGLUT_MotionFunc(x, y);
    glm::vec2 mousePos = glm::vec2(((float)x / windowX) * 2 - 1, (((float)y / windowY) * 2 - 1) * -1);
    if (mouseLeftPush)
    {
        angle -= (mousePos.x - mouseX) * 100;
        mouseX = mousePos.x;
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