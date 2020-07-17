#version 410 core

in PVA
{
	vec3 ldsPosition;
} pvaIn;
out vec4 fragmentColor;

uniform sampler2D tex;

void main ()
{
	fragmentColor = texture(tex, (pvaIn.ldsPosition.xy + 1) * 0.5);
}
