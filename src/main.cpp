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
#define rand_from_n2f_to_2f rand_from_0f_to_1f*4.0f-2.0f
#define rand_from_2f_to_6f rand_from_0f_to_1f*4.0f+2.0f

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

double deltaTime, lastFrame;

std::vector<GameObject*> objects;
DirLighting *lighting;
ShadowMap *shadowMap;

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

    objects.push_back(new Plane(glm::vec3(0.0f, 0.0f, 0.0f), 2.5f));
    objects.front()->connectShader(new Shader("shaders/object.vert", "shaders/object.frag"));
    objects.front()->addTexture("images/king1.png");
    objects.front()->setVertexAttributes();
    objects.front()->setLighting(lighting);
    objects.front()->getShader()->setMat4("view", view);
    objects.front()->getShader()->setMat4("projection", projection);
    objects.front()->getShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    dynamicsWorld->addRigidBody(objects.front()->getRigidBody());

    shadowMap->getShader()->use();
    shadowMap->getShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    while(!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        dynamicsWorld->stepSimulation(deltaTime);
        for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
        {
            btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            btTransform trans;
            if (body && body->getMotionState())
                body->getMotionState()->getWorldTransform(trans);
            else
                trans = obj->getWorldTransform();


            objects[i]->setPosition(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
            objects[i]->setRotate(trans.getRotation().getAngle(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());

            if (trans.getOrigin().getY() < -10.0f)
            {
                trans.setIdentity();
                trans.setOrigin(btVector3(rand_from_n2f_to_2f, rand_from_2f_to_6f, rand_from_n2f_to_2f));
                body->setLinearVelocity(btVector3(0,0,0));
                obj->setWorldTransform(trans);
            }
        }

        glClearColor(0.5f, 0.6f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
        projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);

        for (auto object : objects)
            object->setModelMatrix();

        shadowMap->getShader()->use();
        shadowMap->drawSceneRelateToLighting(objects);

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        for (auto object : objects)
        {
            object->getShader()->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, object->getTexture());
            object->getShader()->setInt("objectTexture", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, shadowMap->getDepthMap());
            object->getShader()->setInt("shadowMap", 1);

            object->getShader()->setMat4("model", object->getModelMatrix());
            object->getShader()->setMat4("projection", projection);
            object->draw(object->getShader());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
            delete body->getMotionState();
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;

    delete lighting;
    delete shadowMap;

    for (auto object : objects)
        delete object;

    return 0;
}


void initGlobals()
{
    camPos = glm::vec3(0.0f, 5.0f, 11.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);

    lighting = new DirLighting();
    lighting->setLightPower(0.3f, 1.0f, 0.5f);
    lighting->setDirLight(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    shadowMap = new ShadowMap("shaders/depthShader.vert", "shaders/depthShader.frag");

    view = glm::mat4(1.0f);
    front.y = glm::tan(glm::radians(PITCH));
    front = glm::normalize(front);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(camPos, camPos + front, up);

    projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);

    float nearPlane = 1.0f, farPlane = 20.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    glm::vec3 lightPosition = lighting->getDirection();
    lightPosition *= -5;
    glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
}

void processKeyboardInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void processMouseInput(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glm::vec3 ray = convertScreenCoordsToWorldRayVector(xpos, ypos);
        glm::vec3 coords = findRayIntersectionWithXYplaneAndXZplane(ray, 0.3f);

        objects.push_back(new Cube(coords, 0.3f));
        objects.back()->connectShader(new Shader("shaders/object.vert", "shaders/object.frag"));
        objects.back()->addTexture("images/dice5.png");
        objects.back()->setVertexAttributes();
        objects.back()->setLighting(lighting);
        objects.back()->getShader()->setVec3("colorIfWhite", glm::vec3(rand_from_0f_to_1f,rand_from_0f_to_1f,rand_from_0f_to_1f));
        objects.back()->getShader()->setMat4("view", view);
        objects.back()->getShader()->setMat4("projection", projection);
        objects.back()->getShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        dynamicsWorld->addRigidBody(objects.back()->getRigidBody());
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

    while (glm::abs(objPos.z - 0.0f) > accuracy & glm::abs(objPos.y - cubeSize) > accuracy)
    {
        if (objPos.z < 0.0f | objPos.y < cubeSize)
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

