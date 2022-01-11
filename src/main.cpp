#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>
#include <ctime>

#include "DirLighting.h"
#include "GameObject.h"
#include "ShadowMap.h"

#define rand_from_0f_to_1f (float)rand()/(float)RAND_MAX

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

double deltaTime, lastFrame;

Card card(glm::vec3(0.0f, 0.0f, 0.0f), 2.5f);
std::vector<std::shared_ptr<Cube>> cubes;

DirLighting lighting;
ShadowMap shadowMap;
Shader shaderObj;

glm::vec3 camPos;
glm::vec3 front;
const float PITCH = -14;

glm::mat4 view;
glm::mat4 projection;
glm::mat4 lightSpaceMatrix;

btDiscreteDynamicsWorld* dynamicsWorld;

void processKeyboardInput(GLFWwindow *window, int key, int scancode, int action, int mods);
void processMouseInput(GLFWwindow *window, int button, int action, int mods);

void initGlobals();
glm::vec3 convertScreenCoordsToWorldRayVector(const double &xpos, const double &ypos);
glm::vec3 findRayIntersectionWithXYplaneAndXZplane(glm::vec3 ray, float cubeSize);

int main()
{
    srand(time(0));
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Test Task Samsung", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, processKeyboardInput);
    glfwSetMouseButtonCallback(window, processMouseInput);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -10, 0));

    initGlobals();

    card.connectShader(Shader(shaderObj.getVertexShaderId(), shaderObj.getFragmentShaderId()));
    card.addTexture("images/king1.png");
    card.setVertexAttributes();
    card.setLighting(lighting);
    card.getShader()->setMat4("view", view);
    card.getShader()->setMat4("projection", projection);
    card.getShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    card.setModelMatrix();

    dynamicsWorld->addRigidBody(card.getRigidBody());

    shadowMap.getShader()->use();
    shadowMap.getShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    while(!glfwWindowShouldClose(window))
    {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        dynamicsWorld->stepSimulation(deltaTime);

        glClearColor(0.5f, 0.6f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
        //projection = glm::mat4(1.0f);
        //projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);

        for (const auto cube : cubes)
            cube->setModelMatrix();

        shadowMap.getShader()->use();
        shadowMap.drawSceneRelateToLighting(cubes);

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        card.getShader()->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, card.getTextureId());
        card.getShader()->setInt("objectTexture", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthMap());
        card.getShader()->setInt("shadowMap", 1);

        card.getShader()->setMat4("model", card.getModelMatrix());
        card.getShader()->setMat4("projection", projection);
        card.draw();

        for (const auto& cube : cubes)
        {
            cube->getShader()->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cube->getTextureId());
            cube->getShader()->setInt("objectTexture", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthMap());
            cube->getShader()->setInt("shadowMap", 1);

            cube->getShader()->setMat4("model", cube->getModelMatrix());
            cube->getShader()->setMat4("projection", projection);
            cube->draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;

    return 0;
}


void initGlobals()
{
    camPos = glm::vec3(0.0f, 5.0f, 11.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);

    lighting.setLightPower(0.3f, 1.0f, 0.5f);
    lighting.setDirLight(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    cubes.reserve(128);
    shaderObj.init("shaders/object.vert", "shaders/object.frag");
    shadowMap.init("shaders/depthShader.vert", "shaders/depthShader.frag");

    view = glm::mat4(1.0f);
    front.y = glm::tan(glm::radians(PITCH));
    front = glm::normalize(front);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(camPos, camPos + front, up);

    projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);

    float nearPlane = 1.0f, farPlane = 20.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    glm::vec3 lightPosition = lighting.getDirection();
    lightPosition *= -5;
    glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
}

void processKeyboardInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void processMouseInput(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glm::vec3 ray = convertScreenCoordsToWorldRayVector(xpos, ypos);
        glm::vec3 coords = findRayIntersectionWithXYplaneAndXZplane(ray, 0.3f);

        const auto cube = std::make_shared<Cube>(coords, 0.3f);
        cube->connectShader(Shader(shaderObj.getVertexShaderId(), shaderObj.getFragmentShaderId()));
        cube->addTexture("images/dice5.png");
        cube->setVertexAttributes();
        cube->setLighting(lighting);
        cube->getShader()->setVec3("colorIfWhite", glm::vec3(rand_from_0f_to_1f,rand_from_0f_to_1f,rand_from_0f_to_1f));
        cube->getShader()->setMat4("view", view);
        cube->getShader()->setMat4("projection", projection);
        cube->getShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        dynamicsWorld->addRigidBody(cube->getRigidBody());
        cubes.emplace_back(cube);
    }
}

glm::vec3 convertScreenCoordsToWorldRayVector(const double &xpos, const double &ypos)
{
    float x = (2.0f * static_cast<float>(xpos)) / SCREEN_WIDTH - 1.0f;
    float y = 1.0f - (2.0f * static_cast<float>(ypos)) / SCREEN_HEIGHT;
    float z = 1.0f;
    glm::vec3 rayNDS = glm::vec3(x, y, z);

    glm::vec4 rayClip = glm::vec4(rayNDS.x, rayNDS.y, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    glm::vec4 rayWorld = (glm::inverse(view) * rayEye);
    return glm::normalize(glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z));
}

glm::vec3 findRayIntersectionWithXYplaneAndXZplane(glm::vec3 ray, float cubeSize)
{
    float scaleRay, lengthRay;
    scaleRay = lengthRay = 11.0f;
    float accuracy = 0.01f;
    glm::vec3 objPos = camPos + ray * lengthRay;

    while (glm::abs(objPos.z - 0.0f) > accuracy && glm::abs(objPos.y - cubeSize) > accuracy)
    {
        if (objPos.z < 0.0f || objPos.y < cubeSize)
        {
            scaleRay /= 2.0f;
            lengthRay -= scaleRay;
            objPos = camPos + ray * lengthRay;
        }
        else
        {
            scaleRay /= 2.0f;
            lengthRay += scaleRay;
            objPos = camPos + ray * lengthRay;
        }
    }
    return objPos;
}
