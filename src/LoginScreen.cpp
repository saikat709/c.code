#include "LoginScreen.hpp"
#include "login.hpp"

LoginScreen::LoginScreen(Font& font, ParticleSystem& particles, Vector2u windowSize)
    : font(font), particles(particles),
      userField(font, "Username", {275, 240}, {250, 40}),
      passField(font, "Password", {275, 300}, {250, 40}, true),
      loginBtn(font, "LOGIN", {275, 370}, {250, 45}),
      registerBtn(font, "Create Account", {275, 430}, {250, 30}),
      statusMsg(font, "", 16),
      title(font, "Welcome Back", 32)
{
    card.setSize({350, 450}); 
    card.setOrigin({175, 225});
    card.setPosition({(float) windowSize.x / 2, (float) windowSize.y / 2});
    card.setFillColor(Color(30, 41, 59, 200));
    card.setOutlineThickness(1);
    card.setOutlineColor(Color(255, 255, 255, 50));

    title.setFillColor(Color::White);
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({titleBounds.position.x + titleBounds.size.x / 2.0f, titleBounds.position.y + titleBounds.size.y / 2.0f});
    title.setPosition({(float)windowSize.x / 2, 150});
    
    statusMsg.setFillColor(Color::Red);
    statusMsg.setPosition({275, 470});
}

AppState LoginScreen::run(RenderWindow& window) {
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                return AppState::EXIT;
            
            userField.handleEvent(*event, window);
            passField.handleEvent(*event, window);

            if (const auto* keyEvent = event->getIf<Event::KeyPressed>()) {
                if (keyEvent->code == Keyboard::Key::Tab) {
                    bool uFocus = userField.getFocused();
                    userField.setFocused(!uFocus);
                    passField.setFocused(uFocus);
                }
            }

            if (loginBtn.isClicked(*event, window)) {
                if (login::authenticate(userField.getString(), passField.getString())) {
                    return AppState::CODE_EDITOR;
                } else {
                    statusMsg.setString("Invalid Username or Password");
                    statusMsg.setFillColor(Color::Red);
                }
            }

            if (registerBtn.isClicked(*event, window)) {
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
