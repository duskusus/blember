#version 330

flat in vec3 blockColor;
varying vec3 worldPos;
uniform float time;
uniform float waterlevel;
out vec4 LFragment;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
void main()
{
    vec3 lightDirection = vec3(1.0, -1.0, 0.0);
    vec3 normal = normalize(cross(dFdx(worldPos), dFdy(worldPos)));
    float brightness = dot(normal, lightDirection);
    brightness = clamp(brightness * brightness, 0.2, 1.0);
    if(worldPos.y < waterlevel)
    {
        LFragment = vec4(vec3(0.0, 0.0, 1.0), 1.0);
        return;
    }
    //float brightness = 1.0;

	LFragment = vec4(blockColor * brightness, 1.0);
}