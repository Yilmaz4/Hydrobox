#version 460 core

struct Particle {
    vec2 pos;
    vec2 vel;
};

layout(std430, binding = 0) volatile buffer ssbo {
    Particle particles[];
};

in flat int pidx;

out vec4 FragColor;

void main() {
    Particle p = particles[pidx];
    //vec3 color = mix(vec3(1.f), vec3(1.f, 0.f, 0.f), length(p.vel) / 4.f);
    FragColor = vec4(1.0);
}
