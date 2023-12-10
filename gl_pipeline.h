#pragma once

#include "defualt_include.h"

class ShaderCode;
class Shader;
class ModelInfo2;
class ModelInfo;
class Material;

class MeshVertex;
class MeshTexture;
class Mesh;
class Model;
class Loader;

class MeshVertex;
class MeshTexture;

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


class CorePipeline
{
public:
    static bool isGameExit;

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

    void GameExit()
    {
        glutLeaveMainLoop();
        isGameExit = true;
    }
};

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

    GameObject();
    GameObject(char* name);
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
    float farValue = 100.0f;

    virtual void BeforeRender();
};


class ModelRenderer : public Component, public std::enable_shared_from_this<ModelRenderer>
{
private:
public:
    std::vector<std::shared_ptr<RenderData>> renderDataList;
    std::vector<Material*> materialList;
    std::vector<ModelInfo2*> modelList;

    void AddRenderData(RenderData* renderData);
    void AddRenderData(std::shared_ptr<RenderData> renderData);
    void AddModel(ModelInfo2* modelInfo);
    void AddMaterial(Material* material);
    void AddMaterialModel(Material* material, ModelInfo2* modelInfo);
    virtual void BeforeRender();
};


class CameraControl : public Component, public std::enable_shared_from_this<CameraControl>
{
private:
public:
    glm::vec3 offsetPosition;
    float offsetDistance = 3.0f;
    glm::vec3 directionAngle = glm::vec3(15*D2R, 45 * D2R, 0);

    glm::vec3 targetPosition;
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