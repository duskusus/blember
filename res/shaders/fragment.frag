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
    vec3 sphereCenter = vec3(0.0, 0.0, -10.0);
    vec2 location = (gl_FragCoord.xy - vec2(1280 / 2, 720 / 2)) / 720;
    vec4 color = vec4(0.0);
    if(length(location) < 0.5) color = location.xyxy;
    LFragment = color;
}