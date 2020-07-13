#include "Controller.h"
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "what" << std::endl;
    Controller* c = new Controller("Raymarching", MVC_USE_DEPTH_BIT);
    glClearColor(1, 0, 0, 1);
    GLuint rbo[1];
    GLuint fbo[1];
    glGenFramebuffers(1, fbo);
    glGenRenderbuffers(1, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo[0]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16F, Controller::newWindowWidth, Controller::newWindowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo[0]);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlitNamedFramebuffer(fbo[0], GL_FRONT, 0, 0, Controller::newWindowWidth, Controller::newWindowHeight, 0, 0, Controller::newWindowWidth, Controller::newWindowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    // glfwSwapBuffers(c->theWindow);
    while(1){};
    // c->run();

	return 0;
}
