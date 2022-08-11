#version 330
in vec3 aPos;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
out vec3 worldpos;
void main() {
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    worldpos = aPos;
}