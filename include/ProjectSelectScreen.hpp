#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.hpp"
#include "UI.hpp"
#include "ParticleSystem.hpp"

using namespace std;
using namespace sf;

class ProjectSelectScreen {
    Font& font;
    ParticleSystem& particles;
    
    RectangleShape card;
    Text title;
    InputField userField;
    InputField passField;
    InputField confirmPassField;
    Button registerBtn;
    Button backBtn;
    Text statusMsg;

public:
    ProjectSelectScreen(Font& font, ParticleSystem& particles, Vector2u windowSize);
    AppState run(RenderWindow& window);
};
