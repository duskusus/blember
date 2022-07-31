#version 330
out vec4 LFragment;
void main() {
    LFragment = vec4(0.0, 0.0, length(vec2(500, 500) - gl_FragCoord.xy) / 100.0, 1.0);
}