#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>

#include "ParticleSystem.hpp"
#include "LoginScreen.hpp"
#include "RegisterScreen.hpp"
#include "CodeEditorScreen.hpp"
#include "ProjectSelectScreen.hpp"
#include "AppState.hpp"

int main() {
    RenderWindow window(VideoMode({800, 600}), "Login System", Style::Titlebar | Style::Close);
    window.setFramerateLimit(60);

    Font font;
    if (!font.openFromFile("C:/Windows/Fonts/segoeui.ttf")) {
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) return 1;
    }

    // Shared resources
    ParticleSystem particles(50, {800, 600});
    
    // Screens
    LoginScreen loginScreen(font, particles, {800, 600});
    RegisterScreen registerScreen(font, particles, {800, 600});
    CodeEditorScreen codeEditorScreen(font, particles, {800, 600});
    ProjectSelectScreen projectSelectScreen(font, particles, {800, 600});

    AppState currentState = AppState::LOGIN;

    // Background gradient (shared)
    VertexArray gradient(PrimitiveType::TriangleStrip, 4);
    gradient[0].position = {0, 0};     gradient[0].color = Color(15, 23, 42);
    gradient[1].position = {0, 600};   gradient[1].color = Color(30, 41, 59);
    gradient[2].position = {800, 0};   gradient[2].color = Color(15, 23, 42);
    gradient[3].position = {800, 600}; gradient[3].color = Color(51, 65, 85);

    while (currentState != AppState::EXIT) {
        if (currentState == AppState::LOGIN) {
            currentState = loginScreen.run(window);
        } else if (currentState == AppState::REGISTER) {
            currentState = registerScreen.run(window);
        } else if (currentState == AppState::CODE_EDITOR) {
            currentState = codeEditorScreen.run(window);
        } else if (currentState == AppState::PROJECT_SELECT){
            currentState = projectSelectScreen(window);
        }
    }

    return 0;
}
