#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>
#include <stdlib.h>
#include <string>
#include <GL/glew.h>
#include "GLFW/glfw3.h"

#define MVC_USE_ACCUM_BIT 1
#define MVC_USE_ALPHA_BIT 2
#define MVC_USE_DEPTH_BIT 4
#define MVC_USE_STENCIL_BIT 8
#define MVC_USE_STEREO_BIT 16
// 2. Modifier masks in event callbacks
#define MVC_SHIFT_BIT 1
#define MVC_CTRL_BIT 2
#define MVC_ALT_BIT 4

class Controller {
    public:
    Controller(const std::string& windowTitle, int rcFlags = 0);
    ~Controller();
    GLFWwindow* theWindow;
    void handleDisplay();

    void run();
	void setRunWaitsForAnEvent(bool b) { runWaitsForAnEvent = b;}
	void setWindowTitle(const std::string& title);

    protected:
    void initializeCallbacksForRC();
	void reportWindowInterfaceVersion(std::ostream& os) const;
	void swapBuffers() const { glfwSwapBuffers(theWindow); }

    private:
    void createWindowAndRC(const std::string& windowTitle, int rcFlags);
    int glClearFlags;
    bool runWaitsForAnEvent;
    static int newWindowWidth, newWindowHeight;
    std::string titleString(const std::string& str);
    void setClearFlags(int rcFlags);

    static void charCB(GLFWwindow* window, unsigned int theChar);
	static void displayCB(GLFWwindow* window);
	static void keyboardCB(GLFWwindow* window, int key, int scanCode, int action, int mods);
	static void mouseFuncCB(GLFWwindow* window, int button, int action, int mods);
	static void mouseMotionCB(GLFWwindow* window, double x, double y);
	static void reshapeCB(GLFWwindow* window, int width, int height);
	static void scrollCB(GLFWwindow* window, double xOffset, double yOffset);

};
#endif