#ifndef OBJECT
#define OBJECT

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include <btBulletDynamicsCommon.h>

#include "Shader.h"
#include "DirLighting.h"
#include "MotionState.h"

class GameObject
{
public:
    GameObject(const glm::vec3& pos, float objSize);
    virtual ~GameObject() {};

    //void connectShader(const std::string& vertexPath, const std::string& fragmentPath);
    void connectShader(Shader* shader);
    Shader* getShader();

    void addTexture(const std::string &imagePath);
    unsigned int getTextureId() const { return textureId; }

    void setVertexAttributes();
    void setLighting(const DirLighting& light);
    virtual void setModelMatrix() = 0;
    glm::mat4 getModelMatrix() const { return model; };

    virtual void setPosition(float x, float y, float z);
    virtual void setPosition(const glm::vec3& aPosition);
    void setRotate(float angle, float xRot, float yRot, float zRot);
    void setColour(float r, float g, float b, float a);
    void setColour(const glm::vec4& aColour);
    glm::vec3 getColour() const { return colour; };

    const std::vector<float> &getVertices() const;
    const std::vector<unsigned int> &getIndices() const;
    void draw() const;

protected:
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    glm::mat4 model;

    float size;
    glm::vec3 position;
    float rotateAngle{ 0.0f };
    glm::vec3 rotatePosition{ 0.f, 0.f, 0.f };
    glm::vec3 colour{ 1.0f, 1.0f, 1.0f };

    GLuint VAO{}, VBO{}, EBO{};
    GLuint textureId{};

    Shader* shaderObject;
    virtual void fillVertices() = 0;
    virtual void fillIndices() = 0;
};

class Card : public GameObject
{
public:
    Card(const glm::vec3& pos, float objSize);
    void setModelMatrix() override;
    btRigidBody* getRigidBody() { return &body; }

private:
    btBoxShape shape;
    btRigidBody body;

    void fillVertices() override;
    void fillIndices() override;
};

class Cube : public GameObject
{
public:
    Cube(const glm::vec3& aPos = glm::vec3(0.0f), float aObjSize = 0.3f);
    void setModelMatrix() override;
    void setPosition(const glm::vec3& aPosition) override;
    btRigidBody* getRigidBody() { return &body; }

private:
    btBoxShape shape;
    MotionState motionState;
    btRigidBody body;

    void fillVertices() override;
    void fillIndices() override;

    btVector3 calculateLocalInertia();
};

#endif // OBJECT
