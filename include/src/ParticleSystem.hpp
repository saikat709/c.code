#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

class ParticleSystem {
    struct Particle {
        sf::Vector2f position;
        sf::Vector2f velocity;
        float radius;
        float alpha;
        float alphaChange;
    };
    
    std::vector<Particle> particles;
    std::mt19937 rng;
    sf::Vector2u windowSize;

public:
    ParticleSystem(unsigned int count, sf::Vector2u size);
    void update();
    void draw(sf::RenderWindow& window);
};
