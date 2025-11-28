#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.hpp"
#include "UI.hpp"
#include "ParticleSystem.hpp"

class RegisterScreen {
    sf::Font& font;
    ParticleSystem& particles;
    
    sf::RectangleShape card;
    sf::Text title;
    InputField userField;
    InputField passField;
    InputField confirmPassField;
    Button registerBtn;
    Button backBtn;
    sf::Text statusMsg;

public:
    RegisterScreen(sf::Font& font, ParticleSystem& particles, sf::Vector2u windowSize);
    AppState run(sf::RenderWindow& window);
};
