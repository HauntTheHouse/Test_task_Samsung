//
// Created by Dell on 22.03.2021.
//

#ifndef TEST_TASK_SAMSUNG_OBJECT_H
#define TEST_TASK_SAMSUNG_OBJECT_H

#include <vector>
#include <set>
#include <string>
#include <glm/glm.hpp>
#include "Shader.h"
#include "DirLighting.h"
#include <btBulletDynamicsCommon.h>


//struct Material
//{
//    glm::vec3 diffuse;
//    glm::vec3 specular;
//    float shininess;
//};

class Object
{
protected:
//    Material material{};
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    glm::mat4 model;

    float size;
    glm::vec3 position;
    float rotateAngle;
    glm::vec3 rotatePosition;

    btRigidBody* body;

    unsigned int VAO{}, VBO{}, EBO{};
    unsigned int texture{};

    Shader *shaderObject{};
    virtual void fillVertices() = 0;
    virtual void fillIndices() = 0;

public:
    Object(glm::vec3 pos, float objSize);
    virtual ~Object() { delete shaderObject; };
    virtual void connectShader(Shader *shader);
    virtual Shader* getShader();
    virtual void addTexture(const std::string &imagePath);
    virtual void setVertexAttributes();
    virtual void setLighting(DirLighting *light);
    virtual void setModelMatrix() = 0;
    virtual unsigned int getTexture() { return texture; }
    virtual glm::mat4 getModelMatrix() { return model; };

    virtual btRigidBody* getRigidBody() { return body; }
    virtual void setPosition(float x, float y, float z);
    virtual void setRotate(float angle, float xRot, float yRot, float zRot);

    virtual std::vector<float> &getVertices();
    virtual std::vector<unsigned int> &getIndices();
    virtual void draw(Shader *shader);
};

class Plane : public Object
{
private:
    void fillVertices() override;
    void fillIndices() override;

public:
    Plane(glm::vec3 pos, float s);
//    void addTexture(const std::string &imagePath) override;
    void setModelMatrix() override;
//    void draw(Shader *shader) override;
};

class Cube : public Object
{
private:
    void fillVertices() override;
    void fillIndices() override;

public:
    Cube(glm::vec3 pos, float objSize);
//    void addTexture(const std::string &imagePath) override;
    void setModelMatrix() override;
//    void draw(Shader *shader) override;
};

#endif //TEST_TASK_SAMSUNG_OBJECT_H
