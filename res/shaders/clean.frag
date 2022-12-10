#version 330

flat in vec3 blockColor;
varying vec3 worldPos;

out vec4 LFragment;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
void main()
{
    const vec3 lightPos = vec3(0.0, 200, 0.0);
    vec3 normal = normalize(cross(dFdx(worldPos), dFdy(worldPos)));
    float brightness = clamp(dot(normal, normalize(lightPos - worldPos)), 0.25, 1.0);


	LFragment = vec4(blockColor * brightness, 1.0);
}