#version 330
in vec3 aPos;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform float time;
out vec3 worldpos;
flat out float instanceID;
const float spacing = 5.0;
void main() {
    vec3 blockpos = vec3(spacing * (gl_InstanceID % 16), spacing * ((gl_InstanceID / 10) % 16), spacing * (gl_InstanceID / 100));
    gl_Position = proj * view * model * vec4(aPos + blockpos, 1.0);
    //gl_Position = proj * view * model * vec4(aPos, 1.0);
    worldpos = aPos + blockpos;
    instanceID = gl_InstanceID / 100.0;
}