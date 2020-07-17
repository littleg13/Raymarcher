#include "SceneElement.h"

float SceneElement::lightPos[4*MAX_NUM_LIGHTS] =
	{
		4.0, 3.0, 4.0, 1.0,
		-4.0, 0.0, 4, 1.0,
		0.0, 0.0, 5.0, 1.0
	};


int SceneElement::lightEnabled[MAX_NUM_LIGHTS] =
	{ 1, 0, 0 };

float SceneElement::lightStrength[3*MAX_NUM_LIGHTS] =
	{
		1,1,1,
		1,1,1,
		1,1,1,
	};

float SceneElement::globalAmbient[] = { 0.4, 0.4, 0.4 };

int SceneElement::actualNumLights = 3;

void SceneElement::establishLightingEnviornment(ShaderIF* shaderIF){
    glUniform1i(shaderIF->ppuLoc("actualNumLights"), actualNumLights);
    glUniform4fv(shaderIF->ppuLoc("lightPos"), MAX_NUM_LIGHTS, lightPos);
	glUniform3fv(shaderIF->ppuLoc("lightStrength"), MAX_NUM_LIGHTS, lightStrength);
	glUniform1iv(shaderIF->ppuLoc("lightEnabled"), MAX_NUM_LIGHTS, lightEnabled);
	glUniform3fv(shaderIF->ppuLoc("globalAmbient"), 1, globalAmbient);
}

void SceneElement::setColor(Primitive &prim, float d, float s, float a, float r, float g, float b, float m_p, float ref = 0.0){
        prim.ka[0] = r * a;
        prim.ks[0] = r * s;
        prim.kd[0] = r * d;
        prim.ka[1] = g * a;
        prim.ks[1] = g * s;
        prim.kd[1] = g * d;
        prim.ka[2] = b * a;
        prim.ks[2] = b * s;
        prim.kd[2] = b * d;
        prim.ka[3] = 1.0;
        prim.ks[3] = 1.0;
        prim.kd[3] = 1.0;
        prim.m = m_p;
        prim.alpha = 1.0;
        prim.ref = ref;
    }