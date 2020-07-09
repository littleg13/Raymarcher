#include "Controller.h"

int Controller::newWindowWidth = 512;
int Controller::newWindowHeight = 512;

Controller::Controller(const std::string& windowTitle, int rcFlags) : theWindow(nullptr)
{

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
		if (runWaitsForAnEvent)
			glfwWaitEvents();
		else
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

	glfwSwapBuffers(theWindow);

	// checkForErrors(std::cout, "Controller::handleDisplay");
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
	// if (curController != nullptr)
	// {
	// 	Controller::MouseButton mButton;
	// 	if (button == GLFW_MOUSE_BUTTON_LEFT)
	// 		mButton = Controller::LEFT_BUTTON;
	// 	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	// 		mButton = Controller::RIGHT_BUTTON;
	// 	else
	// 		mButton = Controller::MIDDLE_BUTTON;
	// 	bool pressed = (action == GLFW_PRESS);
	// 	Controller* c = dynamic_cast<Controller*>(curController);
	// 	c->handleMouseButton(
	// 		mButton, pressed, c->lastPixelPosX, c->lastPixelPosY, mapMods(mods));
	// }
}

void Controller::mouseMotionCB(GLFWwindow* window, double x, double y)
{
	// if (curController != nullptr)
	// {
	// 	Controller* c = dynamic_cast<Controller*>(curController);
	// 	c->lastPixelPosX = static_cast<int>(x + 0.5);
	// 	c->lastPixelPosY = static_cast<int>(y + 0.5);
	// 	c->handleMouseMotion(c->lastPixelPosX, c->lastPixelPosY);
	// }
}

void Controller::scrollCB(GLFWwindow* window, double xOffset, double yOffset)
{
	// dynamic_cast<GLFWController*>(curController)->handleScroll(yOffset > 0.0);
}

void Controller::reshapeCB(GLFWwindow* window, int width, int height)
{
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