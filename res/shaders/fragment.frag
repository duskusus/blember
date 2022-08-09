#version 330
out vec4 LFragment;
varying vec3 normal;
void main() {
    const vec3 light = vec3(-1.0, -1.0, -1.0);
    const float intensity = dot(light, normal);
    LFragment = vec4(intensity, 1.0);
}