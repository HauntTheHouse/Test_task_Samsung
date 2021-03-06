#ifndef DIR_LIGHTING
#define DIR_LIGHTING

class DirLighting
{
public:
    DirLighting() = default;
    void setLightPower(float ambPow, float diffPow, float specPow)
    {
       ambientPow = ambPow; diffusePow = diffPow; specularPow = specPow;
    }
    void setDirLight(glm::vec3 dir, glm::vec3 color)
    {
       direction = dir;
       ambient = ambientPow * color; diffuse = diffusePow * color; specular = specularPow * color;
    }
    glm::vec3 getDirection() const { return direction; }
    glm::vec3 getAmbient() const   { return ambient;   }
    glm::vec3 getDiffuse() const   { return diffuse;   }
    glm::vec3 getSpecular() const  { return specular;  }

protected:
    glm::vec3 direction{};
    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};
    float ambientPow{}, diffusePow{}, specularPow{};
};

#endif // DIR_LIGHTING
