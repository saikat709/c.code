#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.hpp"
#include "UI.hpp"
#include "ParticleSystem.hpp"

using namespace std;
using namespace sf;

class LoginScreen {
    Font& font;
    ParticleSystem& particles;
    
    RectangleShape card;
    Text title;
    InputField userField;
    InputField passField;
    Button loginBtn;
    Button registerBtn; // Button to switch to register screen
    Text statusMsg;

public:
    LoginScreen(Font& font, ParticleSystem& particles, Vector2u windowSize);
    AppState run(RenderWindow& window);
};
