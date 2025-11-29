#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <vector>
#include "ParticleSystem.hpp"
#include "AppState.hpp"
#include "UI.hpp"

using namespace std;
using namespace sf;


class CodeEditorScreen {
    Font& font;
    ParticleSystem& particles;
    
    // UI Components
    RectangleShape editorBox;
    RectangleShape outputBox;
    Text editorLabel;
    Text outputLabel;
    Text codeText;
    Text outputText;
    
    Button runBtn;
    Button clearBtn;
    Button logoutBtn;
    
    // Code content
    string code;
    string output;
    
    // Cursor and editing
    size_t cursorPos = 0;
    RectangleShape cursor;
    Clock cursorBlink;
    
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
    vector<string> splitLines(const string& text);
    
    // Cursor and selection helpers
    size_t getCursorPosFromClick(Vector2f mousePos);
    void moveCursorUp();
    void moveCursorDown();
    void deleteSelection();
    string getSelectedText();
    bool hasSelection();
    void clearSelection();
    void drawSelection(RenderWindow& window);

public:
    CodeEditorScreen(Font& font, ParticleSystem& particles, Vector2u windowSize);
    AppState run(RenderWindow& window);
};
