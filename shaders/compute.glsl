#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Particle {
    vec2 pos;
    vec2 vel;
    uint grid_x;
    uint grid_y;
};

layout(std430, binding = 0) volatile buffer ssbo_in {
    Particle particles[];
};

const float PI = 3.14159265;

uniform float dt;
uniform vec2 screenSize;
uniform vec2 windowSize;
uniform vec2 windowVelocity;
uniform vec2 corner;

uniform float h; // smoothing length

float kernel(float r) {
    float q = abs(r) / h;
    if (q <= 0.5f)
        return 40.f * (6.f * (pow(q, 3.f) - pow(q, 2.f)) + 1.f) / 7 * PI * pow(h, 2.f);
    if (q <= 1.f)
        return 80 * pow(1.f - q, 3.f) / 7 * PI * pow(h, 2.f);
    if (q > 1.f)
        return 0.f;
}

void main() {
    Particle p = particles[gl_GlobalInvocationID.x];
    //p.vel.y += 9.81f * dt;
    p.pos += p.vel * dt;
    
    if (p.pos.x < corner.x) {
        p.pos.x = corner.x;
        p.vel.x = windowVelocity.x - p.vel.x * 0.9f;
    }
    else if (p.pos.x > corner.x + windowSize.x / screenSize.x) {
        p.pos.x = corner.x + windowSize.x / screenSize.x;
        p.vel.x = windowVelocity.x - p.vel.x * 0.9f;
    }
    if (p.pos.y < corner.y) {
        p.pos.y = corner.y;
        p.vel.y = windowVelocity.y - p.vel.y * 0.9f;
    }
    else if (p.pos.y > corner.y + windowSize.y / screenSize.y) {
        p.pos.y = corner.y + windowSize.y / screenSize.y;
        p.vel.y = windowVelocity.y - p.vel.y * 0.9f;
    }

    particles[gl_GlobalInvocationID.x] = p;
}