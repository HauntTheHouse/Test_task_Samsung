#include "GameObject.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define rand_from_0f_to_90f (float)rand()/(float)RAND_MAX*90.0f

GameObject::GameObject(glm::vec3 pos, float objSize) : position(pos), size(objSize) {}

void GameObject::connectShader(Shader *shader)
{
    shaderObject = shader;
}

Shader* GameObject::getShader()
{
    return shaderObject;
}

void GameObject::addTexture(const std::string &imagePath)
{
    glGenTextures(1, &texture);

    stbi_set_flip_vertically_on_load(true);
    int imageWidth, imageHeight, nrComponents;
    unsigned char *data = stbi_load(imagePath.c_str(), &imageWidth, &imageHeight, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << imagePath << std::endl;
        stbi_image_free(data);
    }
//
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, texture);
}

void GameObject::setVertexAttributes()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void GameObject::setLighting(DirLighting *light)
{
    shaderObject->use();
    shaderObject->setVec3("dirLight.direction", light->getDirection());
    shaderObject->setVec3("dirLight.ambient", light->getAmbient());
    shaderObject->setVec3("dirLight.diffuse", light->getDiffuse());
    shaderObject->setVec3("dirLight.specular", light->getSpecular());
}

void GameObject::setPosition(float x, float y, float z)
{
    position = glm::vec3(x, y, z);
}

void GameObject::setRotate(float angle, float xRot, float yRot, float zRot)
{
    rotateAngle = angle;
    rotatePosition = glm::vec3(xRot, yRot, zRot);
}


std::vector<float> &GameObject::getVertices()
{
    return vertices;
}

std::vector<unsigned int> &GameObject::getIndices()
{
    return indices;
}

void GameObject::draw(Shader *shader)
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
}

Plane::Plane(glm::vec3 pos, float s) : GameObject(pos, s)
{
    float ratio = 3.5f / size;
    btCollisionShape* groundShape = new btBoxShape(btVector3(size, 0.0f, size * ratio));

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

    btScalar mass(0.0f);
    btVector3 localInertia(0.0f, 0.0f, 0.0f);

    btDefaultMotionState *myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
    body = new btRigidBody(rbInfo);

    Plane::fillVertices();
    Plane::fillIndices();
}

void Plane::fillVertices()
{
    vertices = {
            -1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f,     0.0f,  0.0f,
            -1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,     0.0f,  1.0f,
             1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,     1.0f,  1.0f,
             1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f,     1.0f,  0.0f,
    };
}

void Plane::fillIndices()
{
    indices = {
            0, 2, 1,
            0, 3, 2,
    };
}
void Plane::setModelMatrix()
{
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    if (glm::abs(rotateAngle) > 0.0001)
        model = glm::rotate(model, rotateAngle, rotatePosition);
    float ratio = 3.5f / size;
    model = glm::scale(model, glm::vec3(size, size, size * ratio));
}


Cube::Cube(glm::vec3 pos, float objSize) : GameObject(pos, objSize)
{
    btCollisionShape* cubeShape = new btBoxShape(btVector3(size, size, size));

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));


    btQuaternion q(btVector3(rand_from_0f_to_90f,rand_from_0f_to_90f,rand_from_0f_to_90f),rand_from_0f_to_90f);
    startTransform.setRotation(q);

    btScalar mass(1.0f);
    btVector3 localInertia(0.0f, 0.0f, 0.0f);
    cubeShape->calculateLocalInertia(mass, localInertia);

    btDefaultMotionState *myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, cubeShape, localInertia);
    body = new btRigidBody(rbInfo);

    Cube::fillVertices();
    Cube::fillIndices();
}

void Cube::fillVertices() {
    vertices = {
            // front side
            -1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     0.0f, 0.0f,
            -1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     0.0f, 1.0f,
             1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     1.0f, 1.0f,
             1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     1.0f, 0.0f,

            // left side
            -1.0f, -1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,     0.0f, 0.0f,
            -1.0f,  1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,     0.0f, 1.0f,
            -1.0f,  1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,     1.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,     1.0f, 0.0f,

            // back side
             1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     0.0f, 0.0f,
             1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     0.0f, 1.0f,
            -1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     1.0f, 0.0f,

            // right side
             1.0f, -1.0f,  1.0f,    1.0f,  0.0f,  0.0f,     0.0f, 0.0f,
             1.0f,  1.0f,  1.0f,    1.0f,  0.0f,  0.0f,     0.0f, 1.0f,
             1.0f,  1.0f, -1.0f,    1.0f,  0.0f,  0.0f,     1.0f, 1.0f,
             1.0f, -1.0f, -1.0f,    1.0f,  0.0f,  0.0f,     1.0f, 0.0f,

             // up side
            -1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,     0.0f, 0.0f,
            -1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,     0.0f, 1.0f,
             1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,     1.0f, 1.0f,
             1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,     1.0f, 0.0f,

             // down side
            -1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,     0.0f, 0.0f,
            -1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,     0.0f, 1.0f,
             1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,     1.0f, 1.0f,
             1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,     1.0f, 0.0f,
    };
}

void Cube::fillIndices() {
    indices = {
            0,  2,  1,
            0,  3,  2,

            4,  6,  5,
            4,  7,  6,

            8,  10, 9,
            8,  11, 10,

            12, 14, 13,
            12, 15, 14,

            16, 18, 17,
            16, 19, 18,

            20, 22, 21,
            20, 23, 22
    };
}

void Cube::setModelMatrix()
{
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    if (glm::abs(rotateAngle) > 0.0001)
        model = glm::rotate(model, rotateAngle, rotatePosition);
    model = glm::scale(model, glm::vec3(size));
}