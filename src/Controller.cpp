#include "Controller.h"

Controller* Controller::curController = nullptr;
int Controller::newWindowWidth = 512;
int Controller::newWindowHeight = 512;
int Controller::n_primitives = 0;

Controller::Controller(const std::string& windowTitle, int rcFlags) : theWindow(nullptr), mouseMotionIsRotate(false)
{
	curController = this;
    if (glfwInit()) {
		int code = glfwGetError(NULL);
		std::cout << "glfwInit failed with error: " << code << std::endl; 
	}
	

	// First create the window and its Rendering Context (RC)
	createWindowAndRC(windowTitle, rcFlags);
}

Controller::~Controller()
{
	if (theWindow != nullptr)
		glfwDestroyWindow(theWindow);


    glfwTerminate();
}

void Controller::run()
{
	if (theWindow == nullptr)
		return;
	while (!glfwWindowShouldClose(theWindow))	
    {
		// if (runWaitsForAnEvent)
		// 	glfwWaitEvents();
		// else
			glfwPollEvents();
		handleDisplay();
	}
	glfwDestroyWindow(theWindow);
	theWindow = nullptr;
}


void Controller::createWindowAndRC(const std::string& windowTitle, int rcFlags)
{
	// The following calls enforce use of only non-deprecated functionality.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if ((rcFlags & MVC_USE_DEPTH_BIT) == 0)
		glfwWindowHint(GLFW_DEPTH_BITS, 0);
	else
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
	setClearFlags(rcFlags);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	int minor = 8; // Start AT LEAST one greater than where you really want to start
	while ((theWindow == nullptr) && (minor > 0))
	{
		minor--;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
		theWindow = glfwCreateWindow(newWindowWidth, newWindowHeight, titleString(windowTitle).c_str(), nullptr, nullptr);
	}
	if (theWindow == nullptr)
	{
		std::cerr << "\n**** COULD NOT CREATE A 4.x RENDERING CONTEXT ****\n\n";
		return;
	}

	glfwMakeContextCurrent(theWindow);
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK){
		fprintf(stderr, "failed to init GLEW");
	}
	if ((rcFlags & MVC_USE_DEPTH_BIT) == 0)
		glDisable(GL_DEPTH_TEST);
	else
		glEnable(GL_DEPTH_TEST);
	initializeCallbacksForRC();
}


void Controller::handleDisplay()
{
	// clear the frame buffer
	glClear(glClearFlags);

	// renderAllModels();
	renderScreen();
    glfwSwapBuffers(theWindow);

	// glfwSwapBuffers(theWindow);

	// checkForErrors(std::cout, "Controller::handleDisplay");
}

void Controller::renderScreen(){
	glUseProgram(computeShader->getShaderPgmID());
    int heightWarp = ceil((float)Controller::newWindowHeight / (float)8);
    int widthWarp = ceil((float)Controller::newWindowWidth / (float)8);
	SceneElement::establishLightingEnviornment(computeShader);
	glUniform1i(computeShader->ppuLoc("width"), Controller::newWindowWidth);
	glUniform1i(computeShader->ppuLoc("height"), Controller::newWindowHeight);
	glUniform1i(computeShader->ppuLoc("n_primitives"), Controller::n_primitives);
	glUniform1f(computeShader->ppuLoc("cam.zoom"), cam.zoom);
	glUniform4fv(computeShader->ppuLoc("cam.lookAt"), 1, cam.lookAt);
	cryph::AffPoint pos = dynamicView * cryph::AffPoint(cam.position);
	float dynamic_pos[4];
	pos.aCoords(dynamic_pos);
	glUniform4fv(computeShader->ppuLoc("cam.position"), 1, dynamic_pos);
	glUniform1f(computeShader->ppuLoc("currentTime"), glfwGetTime());
    glDispatchCompute(widthWarp, heightWarp, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glUseProgram(shaderIF->getShaderPgmID());
	glBindVertexArray(vao[0]);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Controller::createScreenQuad(ShaderIF* quadRenderIF, ShaderIF* c_Shader){
	shaderIF = quadRenderIF;
	computeShader = c_Shader;
	float vertices[4][3] = {
        {-1, -1, 0},
        {2, -1, 0},
        {-1, 1, 0},
        {1, 1, 0},
    };
    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	int numBytesInBuffer = 12 * sizeof(float);
	glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(quadRenderIF->pvaLoc("mcPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(quadRenderIF->pvaLoc("mcPosition"));

	glGenTextures(1, tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Controller::newWindowWidth, Controller::newWindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, tex[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	cam.position[0] = 4.0;
	cam.position[1] = 3.0;
	cam.position[2] = 4.0;
	memset(cam.lookAt, 0, sizeof(cam.lookAt));
	cam.zoom = 2.0;
}

void Controller::initializeCallbacksForRC()
{
	glfwSetWindowSizeCallback(theWindow, reshapeCB);
	glfwSetCharCallback(theWindow, charCB);
	glfwSetKeyCallback(theWindow, keyboardCB);
	glfwSetMouseButtonCallback(theWindow, mouseFuncCB);
	glfwSetScrollCallback(theWindow, scrollCB);
	glfwSetCursorPosCallback(theWindow, mouseMotionCB);
}

void Controller::charCB(GLFWwindow* window, unsigned int theChar)
{
	// if (theChar < 128)
	// {
	// 	GLFWController* c = dynamic_cast<GLFWController*>(curController);
	// 	c->handleAsciiChar(
	// 		static_cast<unsigned char>(theChar), c->lastPixelPosX, c->lastPixelPosY);
	// }
}

void Controller::keyboardCB(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	// if (curController != nullptr)
	// {
	// 	Controller* theC = dynamic_cast<Controller*>(curController);
	// 	if ((key == GLFW_KEY_ESCAPE) && (action != GLFW_PRESS))
	// 		theC->handleAsciiChar(27, theC->lastPixelPosX, theC->lastPixelPosY);
	// }
}

void Controller::mouseFuncCB(GLFWwindow* window, int button, int action, int mods)
{
	if (curController != nullptr)
	{
		MouseButton mButton;
		if (button == GLFW_MOUSE_BUTTON_LEFT){
			mButton = LEFT_BUTTON;
			if(action == GLFW_PRESS)
				curController->mouseMotionIsRotate = true;
			else
				curController->mouseMotionIsRotate = false;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
			mButton = RIGHT_BUTTON;
		else
			mButton = MIDDLE_BUTTON;
		bool pressed = (action == GLFW_PRESS);
	// 	c->handleMouseButton(
	// 		mButton, pressed, c->lastPixelPosX, c->lastPixelPosY, mapMods(mods));
	}
}

void Controller::mouseMotionCB(GLFWwindow* window, double x, double y)
{
	if (curController != nullptr)
	{
		int dx = x - curController->lastPixelPosX;
		int dy = y - curController->lastPixelPosY;
		if(curController->mouseMotionIsRotate){
			double rotX = -160 * dy / double(newWindowHeight);
			double rotY = 160 * dx / double(newWindowWidth);
			curController->dynamicView = cryph::Matrix4x4::xRotationDegrees(rotX) * cryph::Matrix4x4::yRotationDegrees(rotY) * curController->dynamicView;
		}
		curController->lastPixelPosX = static_cast<int>(x + 0.5);
		curController->lastPixelPosY = static_cast<int>(y + 0.5);
	}
}

void Controller::scrollCB(GLFWwindow* window, double xOffset, double yOffset)
{
	if (curController != nullptr)
	{
		float scaleFactor = 1.1;
		if(yOffset > 0.0)
			curController->cam.zoom *= scaleFactor;
		else
			curController->cam.zoom *= 1.0/scaleFactor;
	}
	// dynamic_cast<GLFWController*>(curController)->handleScroll(yOffset > 0.0);
}

void Controller::reshapeCB(GLFWwindow* window, int width, int height)
{
	Controller::newWindowWidth = width;
	Controller::newWindowHeight = height;
	glBindTexture(GL_TEXTURE_2D, curController->tex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Controller::newWindowWidth, Controller::newWindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, curController->tex[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glViewport(0, 0, width, height);
	// redraw();
	// dynamic_cast<GLFWController*>(curController)->handleReshape(width, height);
}


std::string Controller::titleString(const std::string& str)
{
	int lastSlash = str.length() - 1;
	while (lastSlash > 0)
	{
		if (str[lastSlash] == '/')
			return str.substr(lastSlash+1);
		lastSlash--;
	}
	// No slashes - just return original string
	return str;
}

void Controller::setClearFlags(int rcFlags)
{
	glClearFlags = GL_COLOR_BUFFER_BIT;
	if ((rcFlags & MVC_USE_DEPTH_BIT) != 0)
		glClearFlags |= GL_DEPTH_BUFFER_BIT;
	if ((rcFlags & MVC_USE_STENCIL_BIT) != 0)
		glClearFlags |= GL_STENCIL_BUFFER_BIT;
}