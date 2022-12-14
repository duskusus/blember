#version 330

layout (location = 0)in vec3 blockPosition; // position of block
layout (location = 1)in uint blockType; // use to calculate offset into texture
layout (location = 2)in vec3 vertPos; //vertex in block

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float time;

flat out vec3 blockColor;
varying vec3 worldPos;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
void main()
{
	worldPos = vertPos + blockPosition;
	gl_Position = proj * view * model * vec4(worldPos,  1.0);
	float a = float(blockType) * 1e-9 - 0.8;
	blockColor = hsv2rgb(vec3(float(blockType + uint(20)) *2e-8, 0.91, 0.50));
}