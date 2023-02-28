#version 330

layout (location = 0)in vec3 vertPos; // position of block

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
	gl_Position = proj * view * model * vec4(vertPos, 1.0);
	blockColor = hsv2rgb(vec3(gl_Position.y, 1.0, 1.0));
}