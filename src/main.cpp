#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "ParticleSystem.hpp"
#include "LoginScreen.hpp"
#include "RegisterScreen.hpp"
#include "CodeEditorScreen.hpp"
#include "ProjectSelectScreen.hpp"
#include "AppState.hpp"

int HEIGHT = 600;
int WIDTH  = 800;

int main() {
    RenderWindow window(VideoMode({WIDTH, HEIGHT}), "C.CODE", Style::Titlebar | Style::Close | Style::Resize);
    window.setFramerateLimit(60);

    Font font;
    if (!font.openFromFile("C:/Windows/Fonts/segoeui.ttf")) {
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) return 1;
    }

    // Shared
    ParticleSystem particles(50, {WIDTH, HEIGHT});
    
    // Screens
    LoginScreen loginScreen(font, particles, {WIDTH, HEIGHT});
    RegisterScreen registerScreen(font, particles, {WIDTH, HEIGHT});
    CodeEditorScreen codeEditorScreen(font, particles, {WIDTH, HEIGHT});
    ProjectSelectScreen projectSelectScreen(font, particles, {WIDTH, HEIGHT});

    AppState currentState = AppState::PROJECT_SELECT;

    // Background gradient (shared)
    VertexArray gradient(PrimitiveType::TriangleStrip, 4);
    gradient[0].position = {0, 0};     gradient[0].color = Color(15, 23, 42);
    gradient[1].position = {0, HEIGHT};   gradient[1].color = Color(30, 41, 59);
    gradient[2].position = {WIDTH, 0};   gradient[2].color = Color(15, 23, 42);
    gradient[3].position = {WIDTH, HEIGHT}; gradient[3].color = Color(51, 65, 85);

    while (currentState != AppState::EXIT) {
        if (currentState == AppState::LOGIN) {
            currentState = loginScreen.run(window);
        } else if (currentState == AppState::REGISTER) {
            currentState = registerScreen.run(window);
        } else if (currentState == AppState::CODE_EDITOR) {
            currentState = codeEditorScreen.run(window);
        } else if (currentState == AppState::PROJECT_SELECT){
            currentState = projectSelectScreen.run(window);
        }
    }

    return 0;
}