#version 460 core

layout(std430, binding = 0) volatile buffer ssbo {
    vec2 particles[];
};

void main() {
    gl_Position = vec4(particles[gl_VertexID], 0.f, 1.f);
}
