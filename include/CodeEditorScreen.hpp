#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <vector>
#include "ParticleSystem.hpp"
#include "AppState.hpp"
#include "UI.hpp"


class CodeEditorScreen {
    sf::Font& font;
    ParticleSystem& particles;
    
    // UI Components
    sf::RectangleShape editorBox;
    sf::RectangleShape outputBox;
    sf::Text editorLabel;
    sf::Text outputLabel;
    sf::Text codeText;
    sf::Text outputText;
    
    Button runBtn;
    Button clearBtn;
    Button logoutBtn;
    
    // Code content
    std::string code;
    std::string output;
    
    // Cursor and editing
    size_t cursorPos = 0;
    sf::RectangleShape cursor;
    sf::Clock cursorBlink;
    
    // Text selection
    size_t selectionStart = 0;
    size_t selectionEnd = 0;
    bool isSelecting = false;
    int preferredColumn = 0;  // For up/down navigation
    
    // Scrolling
    int codeScrollOffset = 0;
    int outputScrollOffset = 0;
    
    // Helper methods
    void executeCode();
    void updateCodeDisplay();
    void updateOutputDisplay();
    std::vector<std::string> splitLines(const std::string& text);
    
    // Cursor and selection helpers
    size_t getCursorPosFromClick(sf::Vector2f mousePos);
    void moveCursorUp();
    void moveCursorDown();
    void deleteSelection();
    std::string getSelectedText();
    bool hasSelection();
    void clearSelection();
    void drawSelection(sf::RenderWindow& window);

public:
    CodeEditorScreen(sf::Font& font, ParticleSystem& particles, sf::Vector2u windowSize);
    AppState run(sf::RenderWindow& window);
};
