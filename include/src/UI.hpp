#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>

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
    InputField() {}
    InputField(const sf::Font& font, const std::string& placeholder, sf::Vector2f pos, sf::Vector2f size, bool password = false);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void setString(const std::string& str);
    std::string getString() const;
    bool getFocused() const;
    void setFocused(bool focused);
    sf::FloatRect getBounds() const;
    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
};

class Button {
    sf::RectangleShape shape;
    sf::Text label;
    sf::Color idleColor = sf::Color(0, 180, 216);
    sf::Color hoverColor = sf::Color(0, 150, 199);
    sf::Color clickColor = sf::Color(0, 119, 182);
    bool isHovered = false;

public:
    Button() {}
    Button(const sf::Font& font, const std::string& text, sf::Vector2f pos, sf::Vector2f size);
    bool update(const sf::RenderWindow& window);
    bool isClicked(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
};
