#version 330
out vec4 LFragment;
flat in vec4 blockColor;
varying vec3 worldPos;
vec3 hsv2rgb(vec3 c)
{
    if(blockColor.a <= 0.00) discard;
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec3 lightPosition = vec3(0.0, 0.0, 0.0);
    vec3 light = worldPos - lightPosition;
    float lightDistance = length(light);
    light /= (lightDistance * lightDistance);
    vec3 normal = normalize(cross(dFdx(worldPos), dFdy(worldPos)));
    float brightness = dot(normal, light) * 10.0;
    float distanceToCenter = length(gl_FragCoord.xy - vec2(1920 / 2, 1080 / 2)) / (gl_FragCoord.z * 150.0);
    brightness /= distanceToCenter;
    brightness = clamp(brightness, 0.0, 1.0);
    LFragment = vec4(hsv2rgb(vec3(blockColor.x, 1.0, brightness)), blockColor.w);
}