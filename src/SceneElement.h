#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "ShaderIF.h"

static const int MAX_NUM_LIGHTS = 3;

typedef float vec4[4];
enum primitive_type {
    CUBE
};
struct Primitive{
    vec4 position;
    vec4 quaternion;
    vec4 ka;
    vec4 kd;
    vec4 ks;
    float m;
    float alpha;
    float ref;
    uint32_t type;
    float scale;
    float padding[3];
};

class SceneElement {
    public:
        SceneElement();
        static void establishLightingEnviornment(ShaderIF* shaderIF);
        static void setColor(Primitive &prim, float d, float s, float a, float r, float g, float b, float m_p, float ref);

    private:
        static int actualNumLights;
        static float lightPos[4 * MAX_NUM_LIGHTS];
        static int lightEnabled[MAX_NUM_LIGHTS];
	    static float lightStrength[3*MAX_NUM_LIGHTS];
	    static float globalAmbient[3];

};

#endif