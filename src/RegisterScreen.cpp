#include "RegisterScreen.hpp"
#include "json.hpp"
#include <iostream>

using namespace std;
using namespace sf;

RegisterScreen::RegisterScreen(Font& font, ParticleSystem& particles, Vector2u windowSize, NetworkClient& networkClient)
    : font(font), particles(particles), networkClient(networkClient),
      userField(font, "Username", {275, 200}, {250, 40}),
      passField(font, "Password", {275, 260}, {250, 40}, true),
      confirmPassField(font, "Confirm Password", {275, 320}, {250, 40}, true),
      registerBtn(font, "REGISTER", {275, 390}, {250, 45}),
      backBtn(font, "Back to Login", {275, 450}, {250, 30}),
      statusMsg("", font, 16),
      title("Create Account", font, 32)
{
    // Setup Card
    card.setSize({350, 500});
    card.setOrigin({175, 250});
    card.setPosition({(float)windowSize.x / 2, (float)windowSize.y / 2});
    card.setFillColor(Color(30, 41, 59, 200));
    card.setOutlineThickness(1);
    card.setOutlineColor(Color(255, 255, 255, 50));

    title.setFillColor(Color::White);
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({titleBounds.getPosition().x + titleBounds.getSize().x / 2.0f, titleBounds.getPosition().y + titleBounds.getSize().y / 2.0f});
    title.setPosition({(float)windowSize.x / 2, 120});

    statusMsg.setFillColor(Color::Red);
    statusMsg.setPosition({275, 490});
}

AppState RegisterScreen::run(RenderWindow& window) {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                return AppState::EXIT;
            
            userField.handleEvent(event, window);
            passField.handleEvent(event, window);
            confirmPassField.handleEvent(event, window);

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Tab) {
                    if (userField.getFocused()) {
                        userField.setFocused(false); passField.setFocused(true);
                    } else if (passField.getFocused()) {
                        passField.setFocused(false); confirmPassField.setFocused(true);
                    } else {
                        confirmPassField.setFocused(false); userField.setFocused(true);
                    }
                }
            }

            if (registerBtn.isClicked(event, window)) {
                if (userField.getString().empty() || passField.getString().empty()) {
                    statusMsg.setString("Fields cannot be empty");
                    statusMsg.setFillColor(Color::Red);
                } else if (passField.getString() != confirmPassField.getString()) {
                    statusMsg.setString("Passwords do not match");
                    statusMsg.setFillColor(Color::Red);
                } else {
                    json request;
                    request["action"] = "register";
                    request["username"] = userField.getString();
                    request["password"] = passField.getString();
                    request["email"] = ""; // Add email field later if needed

                    json response = networkClient.sendRequest(request);

                    if (response["status"] == "success") {
                        statusMsg.setString("Account Created!");
                        statusMsg.setFillColor(Color::Green);
                        // Optional: Delay or wait for user to click back
                    } else {
                        string msg = response.value("message", "Registration failed");
                        statusMsg.setString(msg);
                        statusMsg.setFillColor(Color::Red);
                    }
                }
            }

            if (backBtn.isClicked(event, window)) {
                return AppState::LOGIN;
            }
        }

        registerBtn.update(window);
        backBtn.update(window);
        particles.update();

        window.clear();
        
        // Draw Gradient
        VertexArray gradient(PrimitiveType::TriangleStrip, 4);
        gradient[0].position = {0, 0}; gradient[0].color = Color(15, 23, 42);
        gradient[1].position = {0, 600}; gradient[1].color = Color(30, 41, 59);
        gradient[2].position = {800, 0}; gradient[2].color = Color(15, 23, 42);
        gradient[3].position = {800, 600}; gradient[3].color = Color(51, 65, 85);
        window.draw(gradient);

        // Draw particles
        particles.draw(window);
        
        window.draw(card);
        window.draw(title);
        
        userField.draw(window);
        passField.draw(window);
        confirmPassField.draw(window);
        registerBtn.draw(window);
        backBtn.draw(window);
        window.draw(statusMsg);

        window.display();
    }
    return AppState::EXIT;
}
