#include "Controller.h"
#include "ShaderIF.h"
#include "SceneElement.h"
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "what" << std::endl;
    Controller* c = new Controller("Raymarching", MVC_USE_DEPTH_BIT);
    GLuint tex[1];
    ShaderIF::ShaderSpec computeSpec[1];
    computeSpec[0].fName = "shaders/ray.csh";
    computeSpec[0].sType = GL_COMPUTE_SHADER;
    ShaderIF* computeShader = new ShaderIF(computeSpec, 1);
    ShaderIF* shaderIF = new ShaderIF("shaders/basic.vsh", "shaders/phong.fsh");
    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    c->createScreenQuad(shaderIF, computeShader);

    Controller::n_primitives = 36;
    Primitive* primitives = new Primitive[Controller::n_primitives];

    int numBytesInBuffer = Controller::n_primitives * sizeof(Primitive);

    for(int i=0;i<Controller::n_primitives;i++){
        for(int j=0;j<3;j++)
            primitives[i].position[j] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 5 - 2.5;
        // primitives[0].position[0] = -0;
        primitives[i].position[3] = 1.0;
        primitives[i].scale = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.3 + 0.25;
        primitives[i].type = primitive_type::CUBE;
        SceneElement::setColor(primitives[i], 0.7, 0.8, 0.2, 0.1, 0.4, 0.9, 10, 0.015);
        std::cout << primitives[i].kd[0] << " " << primitives[i].kd[1] << " " << primitives[i].kd[2] << " " << primitives[i].kd[3] << std::endl;
    }

    GLuint ssbo;

    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, numBytesInBuffer, primitives, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ssbo);


    c->run();

	return 0;
}
