#version 330
out vec4 FragColor;
varying vec3 worldPos;

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
    FragColor = vec4(vec3(brightness, brightness, brightness), 1.0);
}