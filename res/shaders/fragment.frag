#version 330

#define M_PI 3.1415926535897932384626433832795

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


    vec3 pos = vec3(worldPos.xy * 100, 100);

    float d = 1e-3;

    float k1 = M_PI * 2.0 / (6e-2);
    float k2 = M_PI * 2.0 / (6e-2);

    float w1 = 2 * M_PI * 1;
    float w2 = 2 * M_PI * 1.00;

    float a1 = 0;
    float a2 = 3;
    
    vec3 source1 = vec3(d / 2.0, 0.0, 0.0);
    vec3 source2 = vec3(-d / 2.0, 0.0, 0.0);

    float r1 = length(source1 - pos);
    float r2 = length(source2 - pos);

    float sum = a1 * cos(k1 * r1 - time * w1) + a2 * cos(k1 * r2 - time * w2);
    sum = sum * 0.25 + 0.5;

    LFragment = vec4(0.0, sum, 0.0,  1.0);
}