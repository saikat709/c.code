#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

class ParticleSystem {
    struct Particle {
        Vector2f position;
        Vector2f velocity;
        float radius;
        float alpha;
        float alphaChange;
    };
    
    vector<Particle> particles;
    mt19937 rng;
    Vector2u windowSize;

public:
    ParticleSystem(unsigned int count, Vector2u size);
    void update();
    void draw(RenderWindow& window);
};
