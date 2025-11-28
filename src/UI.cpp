#include "UI.hpp"

// --- InputField ---
InputField::InputField(const sf::Font& font, const std::string& placeholder, sf::Vector2f pos, sf::Vector2f size, bool password) 
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

void InputField::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
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

void InputField::draw(sf::RenderWindow& window) {
    window.draw(box);
    if (content.empty()) {
        window.draw(placeholderText);
    } else {
        window.draw(text);
    }
}

std::string InputField::getString() const { return content; }
bool InputField::getFocused() const { return isFocused; }
void InputField::setFocused(bool focused) { 
    isFocused = focused; 
    box.setFillColor(isFocused ? focusColor : baseColor);
    box.setOutlineColor(isFocused ? sf::Color::Cyan : sf::Color(255, 255, 255, 100));
}
sf::FloatRect InputField::getBounds() const { return box.getGlobalBounds(); }

// --- Button ---

Button::Button(const sf::Font& font, const std::string& text, sf::Vector2f pos, sf::Vector2f size) 
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

bool Button::update(const sf::RenderWindow& window) {
    auto mouse = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
    isHovered = shape.getGlobalBounds().contains(mousePos);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && isHovered) {
        shape.setFillColor(clickColor);
        return true; 
    } else {
        shape.setFillColor(isHovered ? hoverColor : idleColor);
        return false;
    }
}

bool Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
            auto mouse = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
            if (shape.getGlobalBounds().contains(mousePos)) return true;
    }
    return false;
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(label);
}
