//
// Created by Dell on 23.03.2021.
//

#ifndef TEST_TASK_SAMSUNG_DIRLIGHTING_H
#define TEST_TASK_SAMSUNG_DIRLIGHTING_H

class DirLighting
{
protected:
    glm::vec3 direction{};
    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};
    float ambientPow{}, diffusePow{}, specularPow{};
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
    glm::vec3 getDirection() { return direction; }
    glm::vec3 getAmbient()   { return ambient;   }
    glm::vec3 getDiffuse()   { return diffuse;   }
    glm::vec3 getSpecular()  { return specular;  }
};

#endif //TEST_TASK_SAMSUNG_DIRLIGHTING_H
