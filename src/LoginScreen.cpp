#include "LoginScreen.hpp"
#include "login.hpp"
#include <iostream>
#include "json.hpp"
#include "Session.hpp"

using namespace std;
using namespace sf;

LoginScreen::LoginScreen(Font& font, ParticleSystem& particles, Vector2u windowSize, NetworkClient& networkClient)
    : font(font), particles(particles), networkClient(networkClient),
      userField(font, "Username", {275, 240}, {250, 40}),
      passField(font, "Password", {275, 300}, {250, 40}, true),
      loginBtn(font, "LOGIN", {275, 370}, {250, 45}),
      registerBtn(font, "Create Account", {275, 430}, {250, 30}),
      statusMsg("", font, 16),
      title("Welcome Back", font, 32)
{
    card.setSize({350, 450}); 
    card.setOrigin({175, 225});
    card.setPosition({(float) windowSize.x / 2, (float) windowSize.y / 2});
    card.setFillColor(Color(30, 41, 59, 200));
    card.setOutlineThickness(1);
    card.setOutlineColor(Color(255, 255, 255, 50));

    title.setFillColor(Color::White);
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({titleBounds.getPosition().x + titleBounds.getSize().x / 2.0f, titleBounds.getPosition().y + titleBounds.getSize().y / 2.0f});
    title.setPosition({(float)windowSize.x / 2, 150});
    
    statusMsg.setFillColor(Color::Red);
    statusMsg.setPosition({275, 470});
}

AppState LoginScreen::run(RenderWindow& window) {
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                return AppState::EXIT;
            
            userField.handleEvent(event, window);
            passField.handleEvent(event, window);
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Tab) {
                    bool uFocus = userField.getFocused();
                    userField.setFocused(!uFocus);
                    passField.setFocused(uFocus);
                }
            }

            if (loginBtn.isClicked(event, window)) {
                json request;
                request["action"] = "login";
                request["username"] = userField.getString();
                request["password"] = passField.getString();

                json response = networkClient.sendRequest(request);
                
                if (response["status"] == "success") {
                    Session::getInstance().setUserId(response["user_id"]);
                    Session::getInstance().setUsername(response["username"]);
                    return AppState::PROJECT_SELECT;
                } else {
                    string msg = response.value("message", "Login failed");
                    statusMsg.setString(msg);
                    statusMsg.setFillColor(Color::Red);
                }
            }

            if (registerBtn.isClicked(event, window)) {
                return AppState::REGISTER;
            }
        }

        loginBtn.update(window);
        registerBtn.update(window);
        particles.update();

        window.clear();
        
        // Gradient
        VertexArray gradient(PrimitiveType::TriangleStrip, 4);
        gradient[0].position = {0, 0}; gradient[0].color = Color(15, 23, 42);
        gradient[1].position = {0, 600}; gradient[1].color = Color(30, 41, 59);
        gradient[2].position = {800, 0}; gradient[2].color = Color(15, 23, 42);
        gradient[3].position = {800, 600}; gradient[3].color = Color(51, 65, 85);
        window.draw(gradient);

        particles.draw(window);
        window.draw(card);
        window.draw(title);
        userField.draw(window);
        passField.draw(window);
        loginBtn.draw(window);
        registerBtn.draw(window);
        window.draw(statusMsg);

        window.display();
    }
    return AppState::EXIT;
}
