#version 330

layout (location = 0)in vec3 vertPos; // position of block
layout (location = 1)in vec3 vertColor;

uniform mat4 pvm;
uniform float time;

varying vec3 worldPos;
varying vec3 color;

void main()
{
	gl_Position = pvm * vec4(vertPos, 1.0);
    worldPos = vertPos;
    color = vertColor;
}