#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
#include <cstdint>
#include "login.hpp"

class ParticleSystem {
    struct Particle {
        sf::Vector2f position;
        sf::Vector2f velocity;
        float radius;
        float alpha;
        float alphaChange;
    };
    
    std::vector<Particle> particles;
    std::mt19937 rng;
    sf::Vector2u windowSize;

public:
    ParticleSystem(unsigned int count, sf::Vector2u size) : windowSize(size) {
        rng.seed(std::random_device()());
        std::uniform_real_distribution<float> distX(0, (float)size.x);
        std::uniform_real_distribution<float> distY(0, (float)size.y);
        std::uniform_real_distribution<float> distVel(-0.5f, 0.5f);
        std::uniform_real_distribution<float> distRad(2.0f, 5.0f);
        std::uniform_real_distribution<float> distAlpha(50.0f, 150.0f);

        for (unsigned int i = 0; i < count; ++i) {
            particles.push_back({
                {distX(rng), distY(rng)},
                {distVel(rng), distVel(rng)},
                distRad(rng),
                distAlpha(rng),
                (distAlpha(rng) > 100 ? -0.2f : 0.2f)
            });
        }
    }

    void update() {
        for (auto& p : particles) {
            p.position += p.velocity;
            p.alpha += p.alphaChange;

            if (p.alpha <= 20 || p.alpha >= 180) p.alphaChange *= -1;

            // Wrap around
            if (p.position.x < 0) p.position.x = windowSize.x;
            if (p.position.x > windowSize.x) p.position.x = 0;
            if (p.position.y < 0) p.position.y = windowSize.y;
            if (p.position.y > windowSize.y) p.position.y = 0;
        }
    }

    void draw(sf::RenderWindow& window) {
        sf::CircleShape shape;
        for (const auto& p : particles) {
            shape.setRadius(p.radius);
            shape.setPosition(p.position);
            shape.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(p.alpha)));
            window.draw(shape);
        }
    }
};

class InputField {
    sf::RectangleShape box;
    sf::Text text;
    sf::Text placeholderText;
    std::string content;
    bool isFocused = false;
    sf::Color baseColor = sf::Color(255, 255, 255, 30);
    sf::Color focusColor = sf::Color(255, 255, 255, 60);
    sf::Color textColor = sf::Color::White;
    bool isPassword = false;

public:
    InputField(const sf::Font& font, const std::string& placeholder, sf::Vector2f pos, sf::Vector2f size, bool password = false) 
        : text(font, "", 18), placeholderText(font, placeholder, 18), isPassword(password) {
        box.setPosition(pos);
        box.setSize(size);
        box.setFillColor(baseColor);
        box.setOutlineThickness(1);
        box.setOutlineColor(sf::Color(255, 255, 255, 100));

        text.setPosition({pos.x + 10, pos.y + 8});
        text.setFillColor(textColor);

        placeholderText.setPosition({pos.x + 10, pos.y + 8});
        placeholderText.setFillColor(sf::Color(200, 200, 200, 150));
    }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        if (event.is<sf::Event::MouseButtonPressed>()) {
            auto mouse = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
            isFocused = box.getGlobalBounds().contains(mousePos);
            box.setFillColor(isFocused ? focusColor : baseColor);
            box.setOutlineColor(isFocused ? sf::Color::Cyan : sf::Color(255, 255, 255, 100));
        }

        if (isFocused) {
            if (const auto* textEvent = event.getIf<sf::Event::TextEntered>()) {
                if (textEvent->unicode == 8) { // Backspace
                    if (!content.empty()) content.pop_back();
                } else if (textEvent->unicode < 128 && textEvent->unicode >= 32) {
                    content += static_cast<char>(textEvent->unicode);
                }
            }
        }
        
        if (isPassword) {
            text.setString(std::string(content.size(), '*'));
        } else {
            text.setString(content);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(box);
        if (content.empty()) {
            window.draw(placeholderText);
        } else {
            window.draw(text);
        }
    }

    std::string getString() const { return content; }
    bool getFocused() const { return isFocused; }
    void setFocused(bool focused) { 
        isFocused = focused; 
        box.setFillColor(isFocused ? focusColor : baseColor);
        box.setOutlineColor(isFocused ? sf::Color::Cyan : sf::Color(255, 255, 255, 100));
    }
    sf::FloatRect getBounds() const { return box.getGlobalBounds(); }
};

class Button {
    sf::RectangleShape shape;
    sf::Text label;
    sf::Color idleColor = sf::Color(0, 180, 216);
    sf::Color hoverColor = sf::Color(0, 150, 199);
    sf::Color clickColor = sf::Color(0, 119, 182);
    bool isHovered = false;

public:
    Button(const sf::Font& font, const std::string& text, sf::Vector2f pos, sf::Vector2f size) 
        : label(font, text, 20) {
        shape.setPosition(pos);
        shape.setSize(size);
        shape.setFillColor(idleColor);

        // Center text
        sf::FloatRect textBounds = label.getLocalBounds();
        label.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f,
                        textBounds.position.y + textBounds.size.y / 2.0f});
        label.setPosition({pos.x + size.x / 2.0f, pos.y + size.y / 2.0f});
        label.setFillColor(sf::Color::White);
    }

    bool update(const sf::RenderWindow& window) {
        auto mouse = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
        isHovered = shape.getGlobalBounds().contains(mousePos);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && isHovered) {
            shape.setFillColor(clickColor);
            return true; // Clicked (continuous, handle single click in event loop if needed)
        } else {
            shape.setFillColor(isHovered ? hoverColor : idleColor);
            return false;
        }
    }

    bool isClicked(const sf::Event& event, const sf::RenderWindow& window) {
        if (event.is<sf::Event::MouseButtonPressed>()) {
             auto mouse = sf::Mouse::getPosition(window);
             sf::Vector2f mousePos(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
             if (shape.getGlobalBounds().contains(mousePos)) return true;
        }
        return false;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(label);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Login System", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/segoeui.ttf")) {
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) return 1;
    }

    sf::VertexArray gradient(sf::PrimitiveType::TriangleStrip, 4);
    gradient[0].position = {0, 0}; gradient[0].color = sf::Color(15, 23, 42);
    gradient[1].position = {0, 600}; gradient[1].color = sf::Color(30, 41, 59);
    gradient[2].position = {800, 0}; gradient[2].color = sf::Color(15, 23, 42);
    gradient[3].position = {800, 600}; gradient[3].color = sf::Color(51, 65, 85);

    ParticleSystem particles(50, {800, 600});

    sf::RectangleShape card;
    card.setSize({350, 400});
    card.setOrigin({175, 200});
    card.setPosition({400, 300});
    card.setFillColor(sf::Color(30, 41, 59, 200));
    card.setOutlineThickness(1);
    card.setOutlineColor(sf::Color(255, 255, 255, 50));

    sf::Text title(font, "Welcome Back", 32);
    title.setFillColor(sf::Color::White);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({titleBounds.position.x + titleBounds.size.x / 2.0f, titleBounds.position.y + titleBounds.size.y / 2.0f});
    title.setPosition({400, 180});

    InputField userField(font, "Username", {275, 240}, {250, 40});
    InputField passField(font, "Password", {275, 300}, {250, 40}, true);
    Button loginBtn(font, "LOGIN", {275, 370}, {250, 45});

    sf::Text statusMsg(font, "", 16);
    statusMsg.setFillColor(sf::Color::Red);
    statusMsg.setPosition({275, 430});

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            
            userField.handleEvent(*event, window);
            passField.handleEvent(*event, window);

            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Tab) {
                    bool uFocus = userField.getFocused();
                    userField.setFocused(!uFocus);
                    passField.setFocused(uFocus);
                }
            }

            if (loginBtn.isClicked(*event, window)) {
                if (login::authenticate(userField.getString(), passField.getString())) {
                    statusMsg.setString("Login Successful!");
                    statusMsg.setFillColor(sf::Color::Green);
                } else {
                    statusMsg.setString("Invalid Username or Password");
                    statusMsg.setFillColor(sf::Color::Red);
                }
            }
        }

        loginBtn.update(window);
        particles.update();

        window.clear();
        window.draw(gradient);
        particles.draw(window);
        
        window.draw(card);
        window.draw(title);
        
        userField.draw(window);
        passField.draw(window);
        loginBtn.draw(window);
        window.draw(statusMsg);

        window.display();
    }

    return 0;
}
