//
// Created by Dell on 27.03.2021.
//

#ifndef TEST_TASK_SAMSUNG_SHADOWMAP_H
#define TEST_TASK_SAMSUNG_SHADOWMAP_H

#include "Shader.h"
#include "DirLighting.h"
#include "Object.h"

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
    void drawSceneRelateToLighting(const std::vector<Object*> &objects);
};


#endif //TEST_TASK_SAMSUNG_SHADOWMAP_H
