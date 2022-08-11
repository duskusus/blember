#version 330
out vec4 LFragment;
varying vec3 worldpos;
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
void main() {
    LFragment = vec4(hsv2rgb(vec3(worldpos.z / 2.5 , 1.0, 1.0)), 1.0);
}