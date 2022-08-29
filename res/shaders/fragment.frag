#version 330
out vec4 LFragment;
flat in vec4 blockColor;
varying vec3 worldPos;
uniform float time;
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
void main() {

    float d = 0.001;

    float k1 = 8230;
    float k2 = 4480;

    float w1 = 22;
    float w2 = 12;

    float a1 = 1;
    float a2 = 1;
    
    vec3 source1 = vec3(d / 2.0, 0.0, 0.0);
    vec3 source2 = vec3(-d / 2.0, 0.0, 0.0);

    float r1 = length(source1 - worldPos);
    float r2 = length(source2 - worldPos);

    float sum = cos(k1 * r1 - time * w1) / (r1) + cos(k1 * r2 - time * w2) / (r2);
    sum = sum * 0.25 + 0.5;

    LFragment = vec4(0.0, sum, 0.0,  1.0);
}