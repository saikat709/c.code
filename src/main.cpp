#include "NetworkClient.hpp"
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "ParticleSystem.hpp"
#include "LoginScreen.hpp"
#include "RegisterScreen.hpp"
#include "CodeEditorScreen.hpp"
#include "ProjectSelectScreen.hpp"
#include "AppState.hpp"

using namespace std;
using namespace sf;

float HEIGHT = 600;
float WIDTH  = 800;

int main() {
    RenderWindow window(VideoMode({(unsigned int)WIDTH, (unsigned int)HEIGHT}), "C.CODE", Style::Titlebar | Style::Close | Style::Resize);
    window.setFramerateLimit(60);

    sf::Font font;
    // Try common Linux fonts
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        if (!font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
            std::cerr << "Failed to load font" << std::endl;
            return 1;
        }
    }

    // Network Client
    NetworkClient networkClient;
    if (!networkClient.connectToServer("127.0.0.1", 8081)) {
        cerr << "Failed to connect to server. Running in offline mode (or exiting?)" << endl;
        // For now, we might want to continue or exit. Let's continue but logging will fail.
    }

    // Shared
    ParticleSystem particles(50, {(unsigned int)WIDTH, (unsigned int)HEIGHT});
    
    // Screens
    LoginScreen loginScreen(font, particles, {(unsigned int)WIDTH, (unsigned int)HEIGHT}, networkClient);
    RegisterScreen registerScreen(font, particles, {(unsigned int)WIDTH, (unsigned int)HEIGHT}, networkClient);
    CodeEditorScreen codeEditorScreen(font, particles, {(unsigned int)WIDTH, (unsigned int)HEIGHT});
    ProjectSelectScreen projectSelectScreen(font, particles, {(unsigned int)WIDTH, (unsigned int)HEIGHT});

    AppState currentState = AppState::LOGIN;

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