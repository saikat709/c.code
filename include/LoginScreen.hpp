#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.hpp"
#include "UI.hpp"
#include "ParticleSystem.hpp"

class LoginScreen {
    sf::Font& font;
    ParticleSystem& particles;
    
    sf::RectangleShape card;
    sf::Text title;
    InputField userField;
    InputField passField;
    Button loginBtn;
    Button registerBtn; // Button to switch to register screen
    sf::Text statusMsg;

public:
    LoginScreen(sf::Font& font, ParticleSystem& particles, sf::Vector2u windowSize);
    AppState run(sf::RenderWindow& window);
};
