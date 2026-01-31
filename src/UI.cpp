#include "UI.hpp"

using namespace std;
using namespace sf;

// --- InputField ---
InputField::InputField(const Font& font, const string& placeholder, Vector2f pos, Vector2f size, bool password) 
    : text("", font, 18), placeholderText(placeholder,  font, 18), isPassword(password) {
    box.setPosition(pos);
    box.setSize(size);
    box.setFillColor(baseColor);
    box.setOutlineThickness(1);
    box.setOutlineColor(Color(255, 255, 255, 100));

    text.setPosition({pos.x + 10, pos.y + 8});
    text.setFillColor(textColor);

    placeholderText.setPosition({pos.x + 10, pos.y + 8});
    placeholderText.setFillColor(Color(200, 200, 200, 150));
}

void InputField::handleEvent(const Event& event, const RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        auto mouse = Mouse::getPosition(window);
        Vector2f mousePos(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
        isFocused = box.getGlobalBounds().contains(mousePos);
        box.setFillColor(isFocused ? focusColor : baseColor);
        box.setOutlineColor(isFocused ? Color::Cyan : Color(255, 255, 255, 100));
    }

    if (isFocused) {
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == 8) { // Backspace
                if (!content.empty()) content.pop_back();
            } else if (event.text.unicode < 128 && event.text.unicode >= 32) {
                content += static_cast<char>(event.text.unicode);
            }
        }
    }
    
    if (isPassword) {
        text.setString(string(content.size(), '*'));
    } else {
        text.setString(content);
    }
}

void InputField::draw(RenderWindow& window) {
    window.draw(box);
    if (content.empty()) {
        window.draw(placeholderText);
    } else {
        window.draw(text);
    }
}

void InputField::setString(const string& str) {
    content = str;
    if (isPassword) {
        text.setString(string(content.size(), '*'));
    } else {
        text.setString(content);
    }
}

string InputField::getString() const { return content; }
bool InputField::getFocused() const { return isFocused; }
void InputField::setFocused(bool focused) { 
    isFocused = focused; 
    box.setFillColor(isFocused ? focusColor : baseColor);
    box.setOutlineColor(isFocused ? Color::Cyan : Color(255, 255, 255, 100));
}
FloatRect InputField::getBounds() const { return box.getGlobalBounds(); }

void InputField::setPosition(Vector2f pos) {
    box.setPosition(pos);
    text.setPosition({pos.x + 10, pos.y + 8});
    placeholderText.setPosition({pos.x + 10, pos.y + 8});
}

void InputField::setSize(Vector2f size) {
    box.setSize(size);
}

// --- Button ---

Button::Button(const Font& font, const string& text, Vector2f pos, Vector2f size) 
    : label(text, font, 20) {
    shape.setPosition(pos);
    shape.setSize(size);
    shape.setFillColor(idleColor);

    // Center text
    FloatRect textBounds = label.getLocalBounds();
    label.setOrigin({textBounds.getPosition().x + textBounds.getSize().x / 2.0f,
                    textBounds.getPosition().y + textBounds.getSize().y / 2.0f});
    label.setPosition({pos.x + size.x / 2.0f, pos.y + size.y / 2.0f});
    label.setFillColor(Color::White);
}

bool Button::update(const RenderWindow& window) {
    auto mouse = Mouse::getPosition(window);
    Vector2f mousePos(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
    isHovered = shape.getGlobalBounds().contains(mousePos);

    if (Mouse::isButtonPressed(Mouse::Button::Left) && isHovered) {
        shape.setFillColor(clickColor);
        return true; 
    } else {
        shape.setFillColor(isHovered ? hoverColor : idleColor);
        return false;
    }
}

bool Button::isClicked(const Event& event, const RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
            auto mouse = Mouse::getPosition(window);
            Vector2f mousePos(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
            if (shape.getGlobalBounds().contains(mousePos)) return true;
    }
    return false;
}

void Button::draw(RenderWindow& window) {
    window.draw(shape);
    window.draw(label);
}

void Button::setPosition(Vector2f pos) {
    shape.setPosition(pos);
    setSize(shape.getSize()); // Re-center text
}

void Button::setSize(Vector2f size) {
    shape.setSize(size);
    Vector2f pos = shape.getPosition();
    FloatRect textBounds = label.getLocalBounds();
    label.setOrigin({textBounds.getPosition().x + textBounds.getSize().x / 2.0f,
                    textBounds.getPosition().y + textBounds.getSize().y / 2.0f});
    label.setPosition({pos.x + size.x / 2.0f, pos.y + size.y / 2.0f});
}
