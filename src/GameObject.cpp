#include "GameObject.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define rand_from_0f_to_90f (float)rand()/(float)RAND_MAX*90.0f
#define rand_from_0f_to_1f (float)rand()/(float)RAND_MAX
#define rand_from_n2f_to_2f rand_from_0f_to_1f*4.0f-2.0f
#define rand_from_2f_to_6f rand_from_0f_to_1f*4.0f+2.0f

GameObject::GameObject(const glm::vec3& pos, float objSize)
    : position(pos)
    , size(objSize)
    , rotateAngle(rand_from_0f_to_90f)
    , rotatePosition(glm::vec3(rand_from_0f_to_90f, rand_from_0f_to_90f, rand_from_0f_to_90f))
{
}

//void GameObject::connectShader(const std::string& vertexPath, const std::string& fragmentPath)
//{
//    shaderObject.init(vertexPath, fragmentPath);
//}

void GameObject::connectShader(Shader* shader)
{
    shaderObject = shader;
}

Shader* GameObject::getShader()
{
    return shaderObject;
}

void GameObject::addTexture(const std::string &imagePath)
{
    glGenTextures(1, &textureId);

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

        glBindTexture(GL_TEXTURE_2D, textureId);
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
}

void GameObject::setVertexAttributes()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
}

void GameObject::setLighting(const DirLighting& light)
{
    shaderObject->use();
    shaderObject->setVec3("dirLight.direction", light.getDirection());
    shaderObject->setVec3("dirLight.ambient", light.getAmbient());
    shaderObject->setVec3("dirLight.diffuse", light.getDiffuse());
    shaderObject->setVec3("dirLight.specular", light.getSpecular());
}

void GameObject::setPosition(float x, float y, float z)
{
    position = glm::vec3(x, y, z);
}

void GameObject::setPosition(const glm::vec3& aPosition)
{
    position = aPosition;
}

void GameObject::setRotate(float angle, float xRot, float yRot, float zRot)
{
    rotateAngle = angle;
    rotatePosition = glm::vec3(xRot, yRot, zRot);
}

void GameObject::setColour(float r, float g, float b, float a)
{
    colour = glm::vec3(r, g, b);

    auto offset{ 8 };
    auto strideSize{ 12 };
    for (size_t i = 0; i < vertices.size(); i += 12)
    {
        vertices[i + offset + 0] = r;
        vertices[i + offset + 1] = g;
        vertices[i + offset + 2] = b;
        vertices[i + offset + 3] = a;
    }
}

void GameObject::setColour(const glm::vec4& aColour)
{
    GameObject::setColour(aColour.r, aColour.g, aColour.b, aColour.a);
}


const std::vector<float>& GameObject::getVertices() const
{
    return vertices;
}

const std::vector<unsigned int>& GameObject::getIndices() const
{
    return indices;
}

void GameObject::draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
}

Card::Card(const glm::vec3& pos, float aSize)
    : GameObject(pos, aSize)
    , shape(btVector3(aSize, 0.0f, 3.5f))
    , body(0.0f, nullptr, &shape)
{
    rotateAngle = 0.0f;
    body.setFriction(static_cast<btScalar>(1.5));

    Card::fillVertices();
    Card::fillIndices();
}

void Card::fillVertices()
{
    vertices = {
            -1.0f, 0.0f,  1.0f,    0.0f, 1.0f, 0.0f,    0.0f,  0.0f,
            -1.0f, 0.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f,  1.0f,
             1.0f, 0.0f, -1.0f,    0.0f, 1.0f, 0.0f,    1.0f,  1.0f,
             1.0f, 0.0f,  1.0f,    0.0f, 1.0f, 0.0f,    1.0f,  0.0f,
    };
}

void Card::fillIndices()
{
    indices = {
            0, 2, 1,
            0, 3, 2,
    };
}
void Card::setModelMatrix()
{
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotateAngle, rotatePosition);
    float ratio = 3.5f / size;
    model = glm::scale(model, glm::vec3(size, size, size * ratio));
}


Cube::Cube(const glm::vec3& pos, float objSize)
    : GameObject(pos, objSize)
    , shape(btVector3(size, size, size))
    , motionState(position, rotateAngle, rotatePosition)
    , body(1.0f, &motionState, &shape, calculateLocalInertia())

{
    Cube::fillVertices();
    Cube::fillIndices();
}

btVector3 Cube::calculateLocalInertia()
{
    btVector3 localInertia(0.0f, 0.0f, 0.0f);
    shape.calculateLocalInertia(1.0f, localInertia);
    return localInertia;
}

void Cube::fillVertices() {
    vertices = {
            // front side
            -1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
            -1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f,
             1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
             1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f,

            // left side
            -1.0f, -1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
            -1.0f,  1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
            -1.0f,  1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

            // back side
             1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
             1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,
            -1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,

            // right side
             1.0f, -1.0f,  1.0f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
             1.0f,  1.0f,  1.0f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
             1.0f,  1.0f, -1.0f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
             1.0f, -1.0f, -1.0f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

             // up side
            -1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
            -1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f,
             1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
             1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,

             // down side
            -1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
            -1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
             1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
             1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
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
    if (position.y < -10.0f)
    {
        body.setLinearVelocity(btVector3(0,0,0));
        body.setWorldTransform(btTransform(btQuaternion::getIdentity(), btVector3(rand_from_n2f_to_2f, rand_from_2f_to_6f, rand_from_n2f_to_2f)));
    }
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotateAngle, rotatePosition);
    model = glm::scale(model, glm::vec3(size));
}

void Cube::setPosition(const glm::vec3& aPosition)
{
    position = aPosition;
    body.setWorldTransform(btTransform(btQuaternion(btVector3(rotatePosition.x, rotatePosition.y, rotatePosition.z), rotateAngle), btVector3(position.x, position.y, position.z)));
}
