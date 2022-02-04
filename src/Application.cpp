#include "Application.h"

#define rand_from_0f_to_1f (float)rand()/(float)RAND_MAX

Application::Application(const glm::ivec2& aScreenResolution, const std::string& aAppName)
	: mScreenResolution(aScreenResolution)
	, mAppName(aAppName)
	, mDeltaTime(0.0)
	, mLastFrame(0.0)
	, mCard(glm::vec3(0.0f, 0.0f, 0.0f), 2.5f)
	, mCubeIter(0)
	//, mShadowMap("shaders/depthShader.vert", "shaders/depthShader.frag")
	//, mShaderObject("shaders/object.vert", "shaders/object.frag")
	, mCamPosition(0.0f, 5.0f, 11.0f)
	, mFront(0.0f, 0.0f, -1.0f)
	, mCollisionConfiguration()
	, mDispatcher(&mCollisionConfiguration)
	, mDbvtBroadphase()
	, mSolver()
	, mDynamicsWorld(&mDispatcher, &mDbvtBroadphase, &mSolver, &mCollisionConfiguration)
{
	srand(static_cast<unsigned int>(time(nullptr)));
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//auto winDeleter = [](GLFWwindow* window)
	//{
	//	glfwDestroyWindow(window);
	//};

	mWindow = std::unique_ptr<GLFWwindow, WindowDeleter>(
		glfwCreateWindow(aScreenResolution.x, aScreenResolution.y, mAppName.c_str(), nullptr, nullptr)
	);

	std::cout << sizeof(mWindow) << std::endl;

	if (!mWindow)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(mWindow.get());

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return;
	}

	glfwSetWindowUserPointer(mWindow.get(), this);
	glfwSetKeyCallback(mWindow.get(), processKeyboardInput);
	glfwSetMouseButtonCallback(mWindow.get(), processMouseInput);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	mDynamicsWorld.setGravity(btVector3(0, -9.8f, 0));

	mLighting.setLightPower(0.3f, 1.0f, 0.5f);
	mLighting.setDirLight(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    mShaderObject.init("shaders/object.vert", "shaders/object.frag");
    mShadowMap.init("shaders/depthShader.vert", "shaders/depthShader.frag");

	mView = glm::mat4(1.0f);
	mFront.y = glm::tan(glm::radians(PITCH));
	mFront = glm::normalize(mFront);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	mView = glm::lookAt(mCamPosition, mCamPosition + mFront, up);

	mProjection = glm::mat4(1.0f);
	mProjection = glm::perspective(glm::radians(45.0f), static_cast<float>(aScreenResolution.x) / static_cast<float>(aScreenResolution.y), 0.1f, 100.0f);

	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
	glm::vec3 lightPosition = mLighting.getDirection();
	lightPosition *= -5;
	glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mLightSpaceMatrix = lightProjection * lightView;


	mCard.connectShader(Shader(mShaderObject.getVertexShaderId(), mShaderObject.getFragmentShaderId()));
	mCard.addTexture("images/king1.png");
	mCard.setVertexAttributes();
	mCard.setLighting(mLighting);
	mCard.getShader()->setMat4("view", mView);
	mCard.getShader()->setMat4("projection", mProjection);
	mCard.getShader()->setMat4("lightSpaceMatrix", mLightSpaceMatrix);
	mCard.setModelMatrix();

	mDynamicsWorld.addRigidBody(mCard.getRigidBody());

	for (auto& cube : mCubes)
	{
		cube.connectShader(Shader(mShaderObject.getVertexShaderId(), mShaderObject.getFragmentShaderId()));
		cube.addTexture("images/dice5.png");
		cube.setVertexAttributes();
		cube.setLighting(mLighting);
		cube.getShader()->setVec3("colorIfWhite", glm::vec3(rand_from_0f_to_1f, rand_from_0f_to_1f, rand_from_0f_to_1f));
		cube.getShader()->setMat4("view", mView);
		cube.getShader()->setMat4("projection", mProjection);
		cube.getShader()->setMat4("lightSpaceMatrix", mLightSpaceMatrix);
		//app->mDynamicsWorld.addRigidBody(cube.getRigidBody());
	}

	mShadowMap.getShader()->use();
	mShadowMap.getShader()->setMat4("lightSpaceMatrix", mLightSpaceMatrix);

	while (!glfwWindowShouldClose(mWindow.get()))
	{
		double currentFrame = glfwGetTime();
		mDeltaTime = currentFrame - mLastFrame;
		mLastFrame = currentFrame;

		mDynamicsWorld.stepSimulation(static_cast<btScalar>(mDeltaTime));

		glClearColor(0.5f, 0.6f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
		//projection = glm::mat4(1.0f);
		//projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);

		for (size_t i = 0; i < mCubeIter; ++i)
			mCubes[i].setModelMatrix();

		mShadowMap.getShader()->use();
		mShadowMap.drawSceneRelateToLighting(mCubes, 64);

		glViewport(0, 0, aScreenResolution.x, aScreenResolution.y);

		mCard.getShader()->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mCard.getTextureId());
		mCard.getShader()->setInt("objectTexture", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mShadowMap.getDepthMap());
		mCard.getShader()->setInt("shadowMap", 1);

		mCard.getShader()->setMat4("model", mCard.getModelMatrix());
		//mCard.getShader()->setMat4("projection", mProjection);
		mCard.draw();

		for (auto& cube : mCubes)
		{
			cube.getShader()->use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, cube.getTextureId());
			cube.getShader()->setInt("objectTexture", 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mShadowMap.getDepthMap());
			cube.getShader()->setInt("shadowMap", 1);

			cube.getShader()->setMat4("model", cube.getModelMatrix());
			cube.getShader()->setMat4("projection", mProjection);
			cube.draw();
		}

		glfwSwapBuffers(mWindow.get());
		glfwPollEvents();
	}
	glfwTerminate();
}

Application::~Application() {};

void Application::processKeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void Application::processMouseInput(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		Application* app = (Application*)glfwGetWindowUserPointer(window);

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glm::vec3 ray = app->convertScreenCoordsToWorldRayVector(xpos, ypos);
		glm::vec3 coords = app->findRayIntersectionWithXYplaneAndXZplane(ray, 0.3f);

		//auto cube = Cube(coords, 0.3f);
		//cube.connectShader(Shader(app->mShaderObject.getVertexShaderId(), app->mShaderObject.getFragmentShaderId()));
		//cube.addTexture("images/dice5.png");
		//cube.setVertexAttributes();
		//cube.setLighting(app->mLighting);
		//cube.getShader()->setVec3("colorIfWhite", glm::vec3(rand_from_0f_to_1f, rand_from_0f_to_1f, rand_from_0f_to_1f));
		//cube.getShader()->setMat4("view", app->mView);
		//cube.getShader()->setMat4("projection", app->mProjection);
		//cube.getShader()->setMat4("lightSpaceMatrix", app->mLightSpaceMatrix);
		//static auto iter = app->mCubes.begin();
		auto& cube = app->mCubes[app->mCubeIter++];
		cube.setPosition(coords);
		cube.setModelMatrix();
		app->mDynamicsWorld.addRigidBody(cube.getRigidBody());
		//iter++;
		//app->mCubes.emplace_back(cube);
		//app->mCubes.
	}
}

glm::vec3 Application::convertScreenCoordsToWorldRayVector(const double& xPos, const double& yPos)
{
	float x = (2.0f * static_cast<float>(xPos)) / mScreenResolution.x - 1.0f;
	float y = 1.0f - (2.0f * static_cast<float>(yPos)) / mScreenResolution.y;
	float z = 1.0f;
	glm::vec3 rayNDS = glm::vec3(x, y, z);

	glm::vec4 rayClip = glm::vec4(rayNDS.x, rayNDS.y, -1.0f, 1.0f);

	glm::vec4 rayEye = glm::inverse(mProjection) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
	glm::vec4 rayWorld = (glm::inverse(mView) * rayEye);
	return glm::normalize(glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z));
}

glm::vec3 Application::findRayIntersectionWithXYplaneAndXZplane(glm::vec3 mRay, float mCubeSize)
{
	float scaleRay, lengthRay;
	scaleRay = lengthRay = 11.0f;
	float accuracy = 0.01f;
	glm::vec3 objPos = mCamPosition + mRay * lengthRay;

	while (glm::abs(objPos.z - 0.0f) > accuracy && glm::abs(objPos.y - mCubeSize) > accuracy)
	{
		if (objPos.z < 0.0f || objPos.y < mCubeSize)
		{
			scaleRay /= 2.0f;
			lengthRay -= scaleRay;
			objPos = mCamPosition + mRay * lengthRay;
		}
		else
		{
			scaleRay /= 2.0f;
			lengthRay += scaleRay;
			objPos = mCamPosition + mRay * lengthRay;
		}
	}
	return objPos;
}
