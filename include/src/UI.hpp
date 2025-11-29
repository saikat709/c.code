#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>

using namespace std;
using namespace sf;

class InputField {
    RectangleShape box;
    Text text;
    Text placeholderText;
    string content;
    bool isFocused = false;
    Color baseColor = Color(255, 255, 255, 30);
    Color focusColor = Color(255, 255, 255, 60);
    Color textColor = Color::White;
    bool isPassword = false;

public:
    InputField(const Font& font, const string& placeholder, Vector2f pos, Vector2f size, bool password = false);
    void handleEvent(const Event& event, const RenderWindow& window);
    void draw(RenderWindow& window);
    string getString() const;
    bool getFocused() const;
    void setFocused(bool focused);
    FloatRect getBounds() const;
};

class Button {
    RectangleShape shape;
    Text label;
    Color idleColor = Color(0, 180, 216);
    Color hoverColor = Color(0, 150, 199);
    Color clickColor = Color(0, 119, 182);
    bool isHovered = false;

public:
    Button(const Font& font, const string& text, Vector2f pos, Vector2f size);
    bool update(const RenderWindow& window);
    bool isClicked(const Event& event, const RenderWindow& window);
    void draw(RenderWindow& window);
};
