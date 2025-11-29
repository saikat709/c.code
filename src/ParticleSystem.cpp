#include "ParticleSystem.hpp"
#include <cstdint>

using namespace std;
using namespace sf;

ParticleSystem::ParticleSystem(unsigned int count, Vector2u size) : windowSize(size) {
    rng.seed(random_device()());
    uniform_real_distribution<float> distX(0, (float)size.x);
    uniform_real_distribution<float> distY(0, (float)size.y);
    uniform_real_distribution<float> distVel(-0.5f, 0.5f);
    uniform_real_distribution<float> distRad(2.0f, 5.0f);
    uniform_real_distribution<float> distAlpha(50.0f, 150.0f);

    for (unsigned int i = 0; i < count; ++i) {
        particles.push_back({
            {distX(rng), distY(rng)},
            {distVel(rng), distVel(rng)},
            distRad(rng),
            distAlpha(rng),
            (distAlpha(rng) > 100 ? -0.2f : 0.2f)
        });
    }
}

void ParticleSystem::update() {
    for (auto& p : particles) {
        p.position += p.velocity;
        p.alpha += p.alphaChange;

        if (p.alpha <= 20 || p.alpha >= 180) p.alphaChange *= -1;

        // Wrap around
        if (p.position.x < 0) p.position.x = windowSize.x;
        if (p.position.x > windowSize.x) p.position.x = 0;
        if (p.position.y < 0) p.position.y = windowSize.y;
        if (p.position.y > windowSize.y) p.position.y = 0;
    }
}

void ParticleSystem::draw(RenderWindow& window) {
    CircleShape shape;
    for (const auto& p : particles) {
        shape.setRadius(p.radius);
        shape.setPosition(p.position);
        shape.setFillColor(Color(255, 255, 255, static_cast<uint8_t>(p.alpha)));
        window.draw(shape);
    }
}
