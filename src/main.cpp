#include "Controller.h"
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "what" << std::endl;
    Controller* c = new Controller("Raymarching", MVC_USE_DEPTH_BIT);


    c->run();

	return 0;
}
