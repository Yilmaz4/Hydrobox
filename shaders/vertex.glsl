#version 460 core

struct Particle {
    vec2 pos;
    vec2 vel;
};

layout(std430, binding = 0) volatile buffer ssbo {
    Particle particles[];
};

out flat int pidx;

uniform vec2 windowSize;
uniform vec2 windowPos;
uniform vec2 windowVelocity;

void main() {
    pidx = gl_VertexID;
    Particle p = particles[pidx];
    vec2 clipSpace = vec2(p.pos.x - (windowPos.x / windowSize.x), p.pos.y - (windowPos.y / windowSize.y)) * 2.f - vec2(1.f);
    gl_Position = vec4(clipSpace.x, -clipSpace.y, 0.f, 1.f);
}
