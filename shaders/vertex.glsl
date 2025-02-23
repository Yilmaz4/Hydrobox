#version 460 core

struct Particle {
    vec2 pos;
    vec2 vel;
};

layout(std430, binding = 0) volatile buffer ssbo {
    Particle particles[];
};

void main() {
    gl_Position = vec4(particles[gl_VertexID].pos, 0.f, 1.f);
}
