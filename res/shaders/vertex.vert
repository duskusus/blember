#version 330
in vec3 aPos;
layout (location = 4) in vec4 aColor;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform float time;
flat out vec4 blockColor;
varying vec3 worldPos;

const int chunkWidth = 16;
const int chunkLength = 16;
const int chunkHeight = 32;

void main() {
    float spacing = 6.0 + 4.0 * cos(time);
    vec3 blockpos = vec3(gl_InstanceID % chunkWidth, (gl_InstanceID / chunkWidth) % chunkLength, gl_InstanceID / (chunkWidth * chunkLength)) * spacing;
    gl_Position = proj * view * model * vec4(aPos + blockpos, 1.0);
    worldPos = gl_Position.xyz;
    //gl_Position = proj * view * model * vec4(aPos, 1.0);
    blockColor = vec4(1.0 + gl_Position.y / 100.0);
}