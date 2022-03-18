#ifndef APPLICATION
#define APPLICATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <array>
#include <memory>

#include "DirLighting.h"
#include "GameObject.h"
#include "ShadowMap.h"

class Application
{
public:
	Application(const glm::ivec2& aScreenResolution, const std::string& aAppName);
	~Application();

	int run();

private:
	struct WindowDeleter
	{
		void operator()(GLFWwindow* window)
		{
			glfwDestroyWindow(window);
		}
	};
	std::unique_ptr<GLFWwindow, WindowDeleter> mWindow;
	glm::ivec2 mScreenResolution;
	std::string mAppName;

	double mDeltaTime, mLastFrame;

	Card mCard;
	std::vector<std::shared_ptr<Cube>> mCubes;

	DirLighting mLighting;
	ShadowMap mShadowMap;
	Shader mShaderObject;

	glm::vec3 mCamPosition;
	glm::vec3 mFront;
	const float PITCH = -14.f;

	glm::mat4 mView;
	glm::mat4 mProjection;
	glm::mat4 mLightSpaceMatrix;

	btDefaultCollisionConfiguration mCollisionConfiguration;
	btCollisionDispatcher mDispatcher;
	btDbvtBroadphase mDbvtBroadphase;
	btSequentialImpulseConstraintSolver mSolver;
	btDiscreteDynamicsWorld mDynamicsWorld;

	static void processKeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void processMouseInput(GLFWwindow* window, int button, int action, int mods);

	glm::vec3 convertScreenCoordsToWorldRayVector(const double& xPos, const double& yPos);
	glm::vec3 findRayIntersectionWithXYplaneAndXZplane(glm::vec3 mRay, float mCubeSize);
};

#endif // APPLICATION