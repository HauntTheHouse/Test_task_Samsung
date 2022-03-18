#ifndef SHADOW_MAP
#define SHADOW_MAP

#include "Shader.h"
#include "DirLighting.h"
#include "GameObject.h"

class ShadowMap
{
private:
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    GLuint depthMapFBO{};
    GLuint depthMap{};

    Shader depthShader;
public:
    ShadowMap() = default;
    ShadowMap(const std::string& vertexPath, const std::string& fragmentPath);
    ~ShadowMap() {}
    void init(const std::string& vertexPath, const std::string& fragmentPath);

    Shader& getShader() { return depthShader; }
    unsigned int getDepthMap() const { return depthMap; }
    void drawSceneRelateToLighting(const std::vector<std::shared_ptr<Cube>>& cubes);
};


#endif // SHADOW_MAP
