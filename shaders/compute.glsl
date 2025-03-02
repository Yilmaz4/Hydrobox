#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Particle {
    vec2 pos;
    vec2 vel;
};

layout(std430, binding = 0) volatile buffer ssbo {
    Particle particles[];
};

uniform float dt;
uniform vec2 windowVelocity;
uniform vec2 corner;

void main() {
    Particle p = particles[gl_GlobalInvocationID.x];
    p.vel.y += 9.81f * dt;
    p.pos += p.vel * dt;
    
    if (p.pos.x < corner.x) {
        p.pos.x = corner.x;
        p.vel.x = windowVelocity.x - p.vel.x * 0.7f;
    }
    else if (p.pos.x > corner.x + 1.f) {
        p.pos.x = corner.x + 1.f;
        p.vel.x = windowVelocity.x - p.vel.x * 0.7f;
    }
    if (p.pos.y < corner.y) {
        p.pos.y = corner.y;
        p.vel.y = windowVelocity.y - p.vel.y * 0.7f;
    }
    else if (p.pos.y > corner.y + 1.f) {
        p.pos.y = corner.y + 1.f;
        p.vel.y = windowVelocity.y - p.vel.y * 0.7f;
    }

    particles[gl_GlobalInvocationID.x] = p;
}