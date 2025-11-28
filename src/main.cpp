#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "ParticleSystem.hpp"
#include "LoginScreen.hpp"
#include "RegisterScreen.hpp"
#include "CodeEditorScreen.hpp"
#include "AppState.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Login System", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/segoeui.ttf")) {
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) return 1;
    }

    // Shared resources
    ParticleSystem particles(50, {800, 600});
    
    // Screens
    LoginScreen loginScreen(font, particles, {800, 600});
    RegisterScreen registerScreen(font, particles, {800, 600});
    CodeEditorScreen codeEditorScreen(font, particles, {800, 600});

    AppState currentState = AppState::LOGIN;

    // Background gradient (shared)
    sf::VertexArray gradient(sf::PrimitiveType::TriangleStrip, 4);
    gradient[0].position = {0, 0}; gradient[0].color = sf::Color(15, 23, 42);
    gradient[1].position = {0, 600}; gradient[1].color = sf::Color(30, 41, 59);
    gradient[2].position = {800, 0}; gradient[2].color = sf::Color(15, 23, 42);
    gradient[3].position = {800, 600}; gradient[3].color = sf::Color(51, 65, 85);

    while (currentState != AppState::EXIT) {
        // Clear with gradient before running screen? 
        // Actually the screens call window.clear(). 
        // We need to pass the gradient to the screens or handle drawing differently.
        // For simplicity, let's modify the screens to NOT clear, or just let them handle it.
        // In the current implementation, screens call window.clear() and window.display().
        // This means they own the frame loop.
        // We should probably pass the gradient to them or have them draw it.
        // Let's just update the screens to draw the gradient.
        // Wait, I didn't pass the gradient to the screens.
        // Let's just let the screens clear with a solid color for now to avoid complexity, 
        // OR better: Move the main loop OUT of the screens and into main(), 
        // and have screens just have update() and draw() methods.
        // BUT, the `run()` method approach is easier for state transitions if we want to block.
        // Let's stick to `run()` but we need to fix the background drawing.
        // I'll update the screens to just clear with a color for now, or I can quickly update them to take the gradient.
        // Actually, the previous main.cpp drew the gradient.
        // Let's update `LoginScreen.cpp` and `RegisterScreen.cpp` to draw the gradient.
        // But I can't easily pass it without changing the signature I just wrote.
        // I'll just recreate the gradient inside the screens for now. It's cheap.
        
        if (currentState == AppState::LOGIN) {
            currentState = loginScreen.run(window);
        } else if (currentState == AppState::REGISTER) {
            currentState = registerScreen.run(window);
        } else if (currentState == AppState::CODE_EDITOR) {
            currentState = codeEditorScreen.run(window);
        }
    }

    return 0;
}
