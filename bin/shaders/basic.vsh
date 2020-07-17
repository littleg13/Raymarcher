#version 430 core


layout (location = 0) in vec3 mcPosition;

out PVA
{
	vec3 ldsPosition;
} pvaOut;

void main ()
{
	// convert current vertex and its associated normal to eye coordinates
	pvaOut.ldsPosition = mcPosition;
	gl_Position = vec4(mcPosition, 1.0);
}
