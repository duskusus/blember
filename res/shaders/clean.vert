#version 330

layout (location = 0)in vec4 blockPosition; // position of block
layout (location = 1)in uint blockType; // use to calculate offset into texture
layout (location = 2)in vec4 vertPos; //vertex in block

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float time;



void main()
{
	gl_Position = proj * view * model * (blockPosition + vertPos);
}