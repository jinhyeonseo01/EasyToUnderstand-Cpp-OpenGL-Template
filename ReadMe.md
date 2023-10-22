
# OpenGL Rendering Pipeline

### 3rd Party
 - **GLEW, GLUT, GLM**
 - **Assimp**

2023 University(한국공학대학교) Graphic APU (2.2 Grade) OpenGL Pipeline Process Develop Project
* * *
# DevLog
## <div align="center"> DevLog_1</div>
Buffer Converter 제작.

![img](./docs/BufferSystem.drawio.png)

## <div align="center"> DevLog_2 
### - Assimp inatall & Develop FBX Loader -
![img](./docs/dev_first_KleeRendering.gif)
<div align="center"> (Example : Gensine Impect - Klee) </div>

```c++
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
```