#ifndef SHADOW_MAP
#define SHADOW_MAP

#include "Shader.h"
#include "DirLighting.h"
#include "GameObject.h"

class ShadowMap
{
private:
//    glm::mat4 lightSpaceMatrix{};
//    DirLighting *lighting;
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    unsigned int depthMapFBO, depthMap;
    Shader *depthShader;
public:
    ShadowMap(const std::string &vertexPath, const std::string &fragmentPath);
    ~ShadowMap() { delete depthShader; }
//    void setLighting(DirLighting *light);
//    void setLightSpaceMatrix(DirLighting *lighting);
    Shader* getShader() { return depthShader; }
    unsigned int getDepthMap() const { return depthMap; }
    void drawSceneRelateToLighting(const std::vector<GameObject*> &objects);
};


#endif // SHADOW_MAP
