#include "RegisterScreen.hpp"

RegisterScreen::RegisterScreen(sf::Font& font, ParticleSystem& particles, sf::Vector2u windowSize)
    : font(font), particles(particles),
      userField(font, "Username", {275, 200}, {250, 40}),
      passField(font, "Password", {275, 260}, {250, 40}, true),
      confirmPassField(font, "Confirm Password", {275, 320}, {250, 40}, true),
      registerBtn(font, "REGISTER", {275, 390}, {250, 45}),
      backBtn(font, "Back to Login", {275, 450}, {250, 30}),
      statusMsg(font, "", 16),
      title(font, "Create Account", 32)
{
    // Setup Card
    card.setSize({350, 500});
    card.setOrigin({175, 250});
    card.setPosition({(float)windowSize.x / 2, (float)windowSize.y / 2});
    card.setFillColor(sf::Color(30, 41, 59, 200));
    card.setOutlineThickness(1);
    card.setOutlineColor(sf::Color(255, 255, 255, 50));

    title.setFillColor(sf::Color::White);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({titleBounds.position.x + titleBounds.size.x / 2.0f, titleBounds.position.y + titleBounds.size.y / 2.0f});
    title.setPosition({(float)windowSize.x / 2, 120});

    statusMsg.setFillColor(sf::Color::Red);
    statusMsg.setPosition({275, 490});
}

AppState RegisterScreen::run(sf::RenderWindow& window) {
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                return AppState::EXIT;
            
            userField.handleEvent(*event, window);
            passField.handleEvent(*event, window);
            confirmPassField.handleEvent(*event, window);

            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Tab) {
                    if (userField.getFocused()) {
                        userField.setFocused(false); passField.setFocused(true);
                    } else if (passField.getFocused()) {
                        passField.setFocused(false); confirmPassField.setFocused(true);
                    } else {
                        confirmPassField.setFocused(false); userField.setFocused(true);
                    }
                }
            }

            if (registerBtn.isClicked(*event, window)) {
                if (userField.getString().empty() || passField.getString().empty()) {
                    statusMsg.setString("Fields cannot be empty");
                    statusMsg.setFillColor(sf::Color::Red);
                } else if (passField.getString() != confirmPassField.getString()) {
                    statusMsg.setString("Passwords do not match");
                    statusMsg.setFillColor(sf::Color::Red);
                } else {
                    statusMsg.setString("Account Created!");
                    statusMsg.setFillColor(sf::Color::Green);
                    // In a real app, save the user here
                }
            }

            if (backBtn.isClicked(*event, window)) {
                return AppState::LOGIN;
            }
        }

        registerBtn.update(window);
        backBtn.update(window);
        particles.update();

        window.clear();
        
        // Draw Gradient
        sf::VertexArray gradient(sf::PrimitiveType::TriangleStrip, 4);
        gradient[0].position = {0, 0}; gradient[0].color = sf::Color(15, 23, 42);
        gradient[1].position = {0, 600}; gradient[1].color = sf::Color(30, 41, 59);
        gradient[2].position = {800, 0}; gradient[2].color = sf::Color(15, 23, 42);
        gradient[3].position = {800, 600}; gradient[3].color = sf::Color(51, 65, 85);
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
