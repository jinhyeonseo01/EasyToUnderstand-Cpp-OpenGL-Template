#pragma once

#include "defualt_include.h"

class ShaderCode;
class Shader;
class ModelInfo;
class ModelInfo;
class Material;

class MeshVertex;
class MeshTexture;
class Mesh;
class Model;
class Loader;
class AssimpPacket;

class MeshVertex;
class ModelTexture;

struct BoneInfo;
struct KeyPosition;
struct KeyRotation;
struct KeyScale;

class Bone;
struct AssimpNodeData;
class Animation;
class Animator;

class UniformSegment;
class AttributeSegment;
class UniformStorage;
class RenderData;
class ModelBufferData;


class CorePipeline;
class World;
class GameObject;
class Component;
class Transform;
class Camera;


class AssimpPacket
{
public:
    std::string name;
    std::shared_ptr<Assimp::Importer> importer;
    std::shared_ptr<aiScene> scene;
    std::shared_ptr<Model> model;
};



#define music_BGM1 0
#define music_BGM2 1

#define music_attack 1

FMOD::Channel* menuMusicChannel = NULL;
int menuMusicIndex = 0;


#define InputMaxCount 0xffff
#define InputMouseMaxCount 0xff

class CorePipeline
{
public:
    static bool isGameExit;

    static float screenX;
    static float screenY;
    static float screenW;
    static float screenH;

    static bool isDebugMode;
    static bool isEditorMode;

    static bool targetFrameLock;
    static float targetFrame;
    static float targetFrameBetween;
    static float deltaTime;
    static float totalTime;
    static std::chrono::steady_clock::time_point updatePrevClock;
    static std::chrono::steady_clock::time_point updateNowClock;
    
    static bool inputDown[InputMaxCount];
    static bool inputStay[InputMaxCount];
    static bool inputUp[InputMaxCount];

    static bool inputSpecDown[InputMaxCount];
    static bool inputSpecStay[InputMaxCount];
    static bool inputSpecUp[InputMaxCount];

    static bool inputMouseDown[InputMouseMaxCount];
    static bool inputMouseUp[InputMouseMaxCount];
    static bool inputMouseStay[InputMouseMaxCount];

    static bool mouseLock;
    static float mouseLockLimit;
    static float mouseMoveSpeedPer;
    static bool mouseReset;
    static bool showMouse;

    static glm::vec2 mousePosition;
    static glm::vec2 mousePositionPrev;
    static glm::vec2 mousePositionLastDown;
    static glm::vec2 mousePositionLastUp;

    static std::map<int, FMOD::Sound*> soundMap;
    static FMOD::System* soundSystem;

    static FMOD::Sound* SoundLoading(int id, LPCSTR path);
    static FMOD::Sound* GetSound(int id);
    static void* extradriverdata;

    static void GameInit()
    {
        for (int i = 0; i < InputMaxCount; i++)
        {
            inputDown[i] = false;
            inputStay[i] = false;
            inputUp[i] = false;
        }
        for (int i = 0; i < InputMaxCount; i++)
        {
            inputSpecDown[i] = false;
            inputSpecStay[i] = false;
            inputSpecUp[i] = false;
        }
        for (int i = 0; i < InputMouseMaxCount; i++)
        {
            inputMouseDown[i] = false;
            inputMouseStay[i] = false;
            inputMouseUp[i] = false;
        }

        FMOD::System_Create(&CorePipeline::soundSystem);
        CorePipeline::soundSystem->init(64, FMOD_INIT_NORMAL, extradriverdata);
    }
    static void SetKeyDown(int key)
    {
        if(inputStay[key] == false)
            inputDown[key] = true;
    }
    static void SetKeySpecDown(int key)
    {
        if (inputSpecStay[key] == false)
            inputSpecDown[key] = true;
    }
    static void SetKeyUp(int key)
    {
        inputUp[key] = true;
    }
    static void SetKeySpecUp(int key)
    {
        inputSpecUp[key] = true;
    }

    static void SetMouseDown(int key)
    {
        if (inputMouseStay[key] == false)
            inputMouseDown[key] = true;
    }
    static void SetMouseUp(int key)
    {
        inputMouseUp[key] = true;
    }

    static bool GetMouseDown(int key)
    {
        return inputMouseDown[key];
    }
    static bool GetMouse(int key)
    {
        return inputMouseStay[key];
    }
    static bool GetMouseUp(int key)
    {
        return inputMouseUp[key];
    }

    static bool GetKeyDown(int key)
    {
        return inputDown[key];
    }
    static bool GetKey(int key)
    {
        return inputStay[key];
    }
    static bool GetKeyUp(int key)
    {
        return inputUp[key];
    }

    static bool GetKeySpecDown(int key)
    {
        return inputSpecDown[key];
    }
    static bool GetKeySpec(int key)
    {
        return inputSpecStay[key];
    }
    static bool GetKeySpecUp(int key)
    {
        return inputSpecUp[key];
    }

    static void InputBeginUpdate()
    {
        for (int i = 0; i < InputMaxCount; i++)
        {
            if(inputDown[i])
                inputStay[i] = true;
        }
        for (int i = 0; i < InputMaxCount; i++)
        {
            if (inputSpecDown[i])
                inputSpecStay[i] = true;
        }
        for (int i = 0; i < InputMouseMaxCount; i++)
        {
            if (inputMouseDown[i])
                inputMouseStay[i] = true;
        }
        if (CorePipeline::mouseLock && CorePipeline::showMouse)
        {
            CorePipeline::showMouse = false;
            glutSetCursor(GLUT_CURSOR_NONE);
        }
        if ((!CorePipeline::mouseLock) && (!CorePipeline::showMouse))
        {
            CorePipeline::showMouse = true;
            glutSetCursor(GLUT_CURSOR_INHERIT);
        }

        if (CorePipeline::GetKeyDown('l'))
        {
            CorePipeline::mouseLock = !CorePipeline::mouseLock;
        }
        bool isPlay = false;
        menuMusicChannel->isPlaying(&isPlay);
        if (!isPlay)
        {
            CorePipeline::soundSystem->playSound(CorePipeline::GetSound(music_BGM1 + menuMusicIndex), 0, false, &menuMusicChannel);
            menuMusicChannel->setVolume(0.1);
            menuMusicIndex = (menuMusicIndex + 1) % 2;
        }
    }
    static void InputAfterUpdate()
    {
        for (int i = 0; i < InputMaxCount; i++)
        {
            inputDown[i] = false;
            if (inputUp[i])
            {
                inputStay[i] = false;
                inputUp[i] = false;
            }
        }
        for (int i = 0; i < InputMaxCount; i++)
        {
            inputSpecDown[i] = false;
            if (inputSpecUp[i])
            {
                inputSpecStay[i] = false;
                inputSpecUp[i] = false;
            }
        }
        for (int i = 0; i < InputMouseMaxCount; i++)
        {
            inputMouseDown[i] = false;
            if (inputMouseUp[i])
            {
                inputMouseStay[i] = false;
                inputMouseUp[i] = false;
            }
        }

        if (mouseLock && glm::distance(glm::vec2(0, 0), CorePipeline::mousePosition) >= CorePipeline::mouseLockLimit)
        {
            SetCursorPos((int)(CorePipeline::screenX + CorePipeline::screenW / 2), (int)(CorePipeline::screenY + CorePipeline::screenH / 2));
            CorePipeline::mouseReset = true;
        }
        else
            mousePositionPrev = mousePosition;
    }
    static void GameExit()
    {
        glutLeaveMainLoop();
        isGameExit = true;
        ShowCursor(true);
    }
};

float CorePipeline::screenX = 0;
float CorePipeline::screenY = 0;

bool CorePipeline::inputDown[InputMaxCount] = {false, };
bool CorePipeline::inputStay[InputMaxCount] = { false, };
bool CorePipeline::inputUp[InputMaxCount] = { false, };

bool CorePipeline::inputSpecDown[InputMaxCount] = { false, };
bool CorePipeline::inputSpecStay[InputMaxCount] = { false, };
bool CorePipeline::inputSpecUp[InputMaxCount] = { false, };

bool CorePipeline::inputMouseDown[InputMouseMaxCount] = { false, };
bool CorePipeline::inputMouseUp[InputMouseMaxCount] = { false, };
bool CorePipeline::inputMouseStay[InputMouseMaxCount] = { false, };

bool CorePipeline::showMouse = true;
bool CorePipeline::mouseLock = true;
bool CorePipeline::mouseReset = false;
float CorePipeline::mouseLockLimit = 0.5f;
float CorePipeline::mouseMoveSpeedPer = 2.0f;

glm::vec2 CorePipeline::mousePosition = glm::vec2(0, 0);
glm::vec2 CorePipeline::mousePositionPrev = glm::vec2(0, 0);
glm::vec2 CorePipeline::mousePositionLastDown = glm::vec2(0, 0);
glm::vec2 CorePipeline::mousePositionLastUp = glm::vec2(0, 0);

std::map<int, FMOD::Sound*> CorePipeline::soundMap = {};
FMOD::System* CorePipeline::soundSystem = nullptr;
void* CorePipeline::extradriverdata = nullptr;

FMOD::Sound* CorePipeline::SoundLoading(int id, LPCSTR path)
{
    FMOD::Sound* result;
    CorePipeline::soundSystem->createSound(path, FMOD_LOOP_OFF, 0, &result);
    soundMap[id] = result;
    return result;
}

FMOD::Sound* CorePipeline::GetSound(int id)
{
    if (CorePipeline::soundMap.find(id) == soundMap.end())
        return NULL;
    return soundMap[id];
}


class World : public std::enable_shared_from_this<World>
{
public:
    std::vector<std::shared_ptr<GameObject>> gameObjectList;
    std::vector<std::shared_ptr<RenderData>> renderingList;
    std::shared_ptr<UniformStorage> uniformStorage;

    std::weak_ptr<GameObject> mainCameraObject;
    std::weak_ptr<Camera> mainCamera;

    World();

    virtual void Init();

    std::weak_ptr<GameObject> CreateGameObject();
    std::weak_ptr<GameObject> CreateGameObject(const char* name);
    bool AddGameObject(std::shared_ptr<GameObject> gameObject);
    
    std::weak_ptr<GameObject> FindGameObjectByName(const char* name);
    std::vector<std::weak_ptr<GameObject>> FindGameObjectsByName(const char* name);

    void WorldUpdate();
    void WorldRender();
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
    bool activePrev = false;
    bool active = true;
    bool destroy = false;

    std::shared_ptr<char> name;

    std::weak_ptr<World> world;

    std::weak_ptr<GameObject> parent;
    std::vector<std::weak_ptr<GameObject>> childs;

    std::weak_ptr<Transform> transform;
    std::vector<std::shared_ptr<Component>> componentList;
    std::shared_ptr<UniformStorage> uniformStorage;

    GameObject();
    GameObject(char* name);
    template <class T>
    std::weak_ptr<T> AddComponent(T* component);
    template <class T>
    std::weak_ptr<T> GetComponent();
    template <class T>
    std::vector<std::weak_ptr<T>> GetComponents();

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
    glm::vec3 scale = glm::vec3(1, 1, 1);

    glm::mat4 GetModelToWorld();
    glm::mat4 GetWorldToModel();

    glm::mat4&& GetModelToWorldAll();
    glm::mat4&& GetWorldToModelAll();
};


class Camera : public Component, public std::enable_shared_from_this<Camera>
{
private:
public:
    float fovy = 60.0;
    float aspect = CorePipeline::screenW / CorePipeline::screenH;
    float nearValue = 0.03f;
    float farValue = 5000.0f;

    virtual void BeforeRender();

    glm::vec3 GetScreenToWorld(glm::vec2 screenPos);
    glm::vec2 GetWorldToScreen(glm::vec3 worldPos);

    glm::vec3 GetProjectionToWorld(glm::vec2 screenPos);
    glm::vec2 GetWorldToProjection(glm::vec3 worldPos);

    void ShackUpdate(glm::vec3& pos, glm::vec3& rotate);
};


class ModelRenderer : public Component, public std::enable_shared_from_this<ModelRenderer>
{
private:
public:
    std::vector<std::shared_ptr<RenderData>> renderDataList;
    std::vector<Material*> materialList;
    std::vector<ModelInfo*> modelList;

    void AddRenderData(RenderData* renderData);
    void AddRenderData(std::shared_ptr<RenderData> renderData);
    void AddModel(ModelInfo* modelInfo);
    void AddMaterial(Material* material);
    void AddMaterialModel(Material* material, ModelInfo* modelInfo);
    virtual void BeforeRender();
};


class CameraControl : public Component, public std::enable_shared_from_this<CameraControl>
{
private:
public:
    glm::vec3 offsetPosition;
    float offsetDistance = 2.5f;
    glm::vec3 directionAngle = glm::vec3(15*D2R, 45 * D2R, 0);

    glm::vec3 targetPosition;
    glm::vec3 currentPosition;
    std::weak_ptr<GameObject> targetObject;

    void SetTarget(std::weak_ptr<GameObject> gameObject);
    
    
    virtual void Create() {}
    virtual void Start();
    virtual void Update();
    virtual void LateUpdate() {}
    virtual void Destroy() {}
    virtual void Enable() {}
    virtual void Disable() {}
    virtual void BeforeRender() {}
};


class AnimationController : public Component, public std::enable_shared_from_this<AnimationController>
{
private:
public:
    std::shared_ptr<Animator> animator;
    std::vector<std::shared_ptr<Animation>> animationList;

    void SetAnimator(std::shared_ptr<AssimpPacket> model);

    virtual void Create() {}
    virtual void Start();
    virtual void Update();
    virtual void LateUpdate();
    virtual void Destroy() {}
    virtual void Enable() {}
    virtual void Disable() {}
    virtual void BeforeRender() {}
};


#define Player_Idle 0
#define Player_WalkStart 1
#define Player_Walk 2
#define Player_WalkEnd 3
#define Player_Turn 4
#define Player_Sliding 5
#define Player_Atack_1 6
#define Player_Atack_1_End 7
#define Player_Atack_2 8
#define Player_Atack_2_End 9
#define Player_Atack_3 10
#define Player_Atack_3_End 11
#define Player_Atack_4 12
#define Player_Atack_4_End 13
#define Player_Atack_5 14
#define Player_Atack_5_End 15

class PlayerController : public Component, public std::enable_shared_from_this<PlayerController>
{
private:
public:
    int animState = Player_Idle;

    glm::vec3 prevAnimPos;
    std::vector<char> keyStack;
    float finalAngle = 0;
    float beforeAngle = 0;
    float smoothAngle = 0;
    float currentAngle = 0;
    float delta = 0;
    float attackCycle = 0;
    float attackTime = 0.35;

    bool turnStart = false;
    bool turn = false;
    bool sliding = false;
    bool attack = false;
    bool walk = false;

    virtual void Create() {}
    virtual void Start();
    virtual void Update();
    virtual void LateUpdate() {}
    virtual void Destroy() {}
    virtual void Enable() {}
    virtual void Disable() {}
    virtual void BeforeRender() {}
};



#define EnemyGiant_Idle 0
#define EnemyGiant_Walk 1
#define EnemyGiant_Attack 1
class EnemyGiantController : public Component, public std::enable_shared_from_this<EnemyGiantController>
{
private:
public:
    int animState = EnemyGiant_Idle;
    std::weak_ptr<GameObject> targetObject;
    glm::vec3 prevAnimPos;
    std::vector<char> keyStack;
    float finalAngle = 0;
    float beforeAngle = 0;
    float smoothAngle = 0;
    float currentAngle = 0;
    float delta = 0;
    float attackCycle = 0;
    float attackTime = 0.5;

    bool turnStart = false;
    bool turn = false;
    bool attack = false;
    bool walk = false;

    virtual void Create() {}
    virtual void Start();
    virtual void Update();
    virtual void LateUpdate() {}
    virtual void Destroy() {}
    virtual void Enable() {}
    virtual void Disable() {}
    virtual void BeforeRender() {}
};


class LightComponent : public Component, public std::enable_shared_from_this<LightComponent>
{
private:
public:

    glm::vec3 lightDirection = glm::vec3(1, -1, 1);
    glm::vec3 lightColor = glm::vec3(1, 1, 1);
    float lightPower = 1.0f;

    virtual void Create() {}
    virtual void Start();
    virtual void Update();
    virtual void LateUpdate() {}
    virtual void Destroy() {}
    virtual void Enable() {}
    virtual void Disable() {}
    virtual void BeforeRender();
};


class FaceSDFComponent : public Component, public std::enable_shared_from_this<FaceSDFComponent>
{
private:
public:
    Material* material;
    virtual void Create() {}
    virtual void Start();
    virtual void Update();
    virtual void LateUpdate() {}
    virtual void Destroy() {}
    virtual void Enable() {}
    virtual void Disable() {}
    virtual void BeforeRender();
};