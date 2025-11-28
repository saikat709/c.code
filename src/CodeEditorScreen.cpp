#include "CodeEditorScreen.hpp"
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <array>
#include <memory>
#include "UI.hpp"
#include <SFML/Graphics.hpp>


CodeEditorScreen::CodeEditorScreen(sf::Font& font, ParticleSystem& particles, sf::Vector2u windowSize)
    : font(font), particles(particles),
      runBtn(font, "RUN", {650, 20}, {120, 40}),
      clearBtn(font, "CLEAR", {650, 70}, {120, 40}),
      logoutBtn(font, "LOGOUT", {650, 520}, {120, 40}),
      editorLabel(font, "Code Editor", 18),
      outputLabel(font, "Output", 18),
      codeText(font, "", 14),
      outputText(font, "", 14)
{
    // Editor Box (left side, top half)
    editorBox.setSize({600, 280});
    editorBox.setPosition({20, 50});
    editorBox.setFillColor(sf::Color(30, 41, 59, 220));
    editorBox.setOutlineThickness(2);
    editorBox.setOutlineColor(sf::Color(100, 150, 200));
    
    // Output Box (left side, bottom half)
    outputBox.setSize({600, 230});
    outputBox.setPosition({20, 350});
    outputBox.setFillColor(sf::Color(20, 30, 45, 220));
    outputBox.setOutlineThickness(2);
    outputBox.setOutlineColor(sf::Color(150, 100, 100));
    
    // Labels
    editorLabel.setFillColor(sf::Color::White);
    editorLabel.setPosition({25, 25});
    
    outputLabel.setFillColor(sf::Color::White);
    outputLabel.setPosition({25, 325});
    
    // Code Text
    codeText.setFillColor(sf::Color(220, 220, 220));
    codeText.setPosition({30, 60});
    
    // Output Text
    outputText.setFillColor(sf::Color(200, 255, 200));
    outputText.setPosition({30, 360});
    
    // Cursor
    cursor.setSize({2, 16});
    cursor.setFillColor(sf::Color::White);
    
    // Default code template
    code = "#include <iostream>\nusing namespace std;\n\nint main() {\n    cout << \"Hello, World!\" << endl;\n    return 0;\n}\n";
    cursorPos = code.length();
    updateCodeDisplay();
}

std::vector<std::string> CodeEditorScreen::splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::stringstream ss(text);
    std::string line;
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    return lines;
}

void CodeEditorScreen::updateCodeDisplay() {
    codeText.setString(code);
    
    // Update cursor position using SFML's text metrics
    sf::Vector2f pos = codeText.findCharacterPos(cursorPos);
    cursor.setPosition(pos);
}

void CodeEditorScreen::updateOutputDisplay() {
    outputText.setString(output);
}

void CodeEditorScreen::executeCode() {
    output = "Compiling...\n";
    updateOutputDisplay();
    
    // Write code to temporary file
    std::ofstream tempFile("temp_code.cpp");
    if (!tempFile) {
        output = "Error: Could not create temporary file\n";
        updateOutputDisplay();
        return;
    }
    tempFile << code;
    tempFile.close();
    
    // Compile the code
    std::string compileCmd = "g++ -std=c++17 temp_code.cpp -o temp_program 2>&1";
    
    std::array<char, 128> buffer;
    std::string compileOutput;
    
    #ifdef _WIN32
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(compileCmd.c_str(), "r"), _pclose);
    #else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(compileCmd.c_str(), "r"), pclose);
    #endif
    
    if (!pipe) {
        output = "Error: Could not execute compiler\n";
        updateOutputDisplay();
        return;
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        compileOutput += buffer.data();
    }
    
    if (!compileOutput.empty()) {
        output = "Compilation Errors:\n" + compileOutput;
        updateOutputDisplay();
        return;
    }
    
    // Run the compiled program
    output = "Running...\n";
    updateOutputDisplay();
    
    #ifdef _WIN32
    std::string runCmd = "temp_program.exe 2>&1";
    std::unique_ptr<FILE, decltype(&_pclose)> runPipe(_popen(runCmd.c_str(), "r"), _pclose);
    #else
    std::string runCmd = "./temp_program 2>&1";
    std::unique_ptr<FILE, decltype(&pclose)> runPipe(popen(runCmd.c_str(), "r"), pclose);
    #endif
    
    if (!runPipe) {
        output = "Error: Could not execute program\n";
        updateOutputDisplay();
        return;
    }
    
    std::string programOutput;
    while (fgets(buffer.data(), buffer.size(), runPipe.get()) != nullptr) {
        programOutput += buffer.data();
    }
    
    output = "Output:\n" + programOutput;
    if (output == "Output:\n") {
        output = "Output:\n(Program executed successfully with no output)";
    }
    updateOutputDisplay();
    
    // Cleanup
    std::remove("temp_code.cpp");
    #ifdef _WIN32
    std::remove("temp_program.exe");
    #else
    std::remove("temp_program");
    #endif
}

bool CodeEditorScreen::hasSelection() {
    return selectionStart != selectionEnd;
}

void CodeEditorScreen::clearSelection() {
    selectionStart = selectionEnd = cursorPos;
}

std::string CodeEditorScreen::getSelectedText() {
    if (!hasSelection()) return "";
    size_t start = std::min(selectionStart, selectionEnd);
    size_t end = std::max(selectionStart, selectionEnd);
    return code.substr(start, end - start);
}

void CodeEditorScreen::deleteSelection() {
    if (!hasSelection()) return;
    size_t start = std::min(selectionStart, selectionEnd);
    size_t end = std::max(selectionStart, selectionEnd);
    code.erase(start, end - start);
    cursorPos = start;
    clearSelection();
}

size_t CodeEditorScreen::getCursorPosFromClick(sf::Vector2f mousePos) {
    // Check if click is within editor bounds (roughly)
    if (mousePos.x < 20 || mousePos.x > 620 || mousePos.y < 50 || mousePos.y > 330) {
        return cursorPos; 
    }
    
    float lineHeight = font.getLineSpacing(14);
    float relativeY = mousePos.y - codeText.getPosition().y;
    int lineIndex = static_cast<int>(relativeY / lineHeight);
    
    auto lines = splitLines(code);
    if (lineIndex < 0) lineIndex = 0;
    if (lineIndex >= static_cast<int>(lines.size())) lineIndex = lines.size() - 1;
    
    // Find start index of this line
    size_t currentPos = 0;
    for (int i = 0; i < lineIndex; ++i) {
        currentPos += lines[i].length() + 1; // +1 for newline
    }
    
    // Now iterate characters in this line to find closest X
    std::string& line = lines[lineIndex];
    float bestDist = 100000.0f;
    size_t bestOffset = 0;
    
    // Check all character positions including the one after the last char
    for (size_t i = 0; i <= line.length(); ++i) {
        sf::Vector2f charPos = codeText.findCharacterPos(currentPos + i);
        float dist = std::abs(mousePos.x - charPos.x);
        if (dist < bestDist) {
            bestDist = dist;
            bestOffset = i;
        }
    }
    
    return currentPos + bestOffset;
}

void CodeEditorScreen::moveCursorUp() {
    auto lines = splitLines(code);
    if (lines.empty()) return;
    
    // Find current line and position within line
    size_t pos = 0;
    int currentLine = 0;
    int columnPos = 0;
    
    for (size_t i = 0; i < lines.size(); i++) {
        if (pos + lines[i].length() >= cursorPos) {
            currentLine = i;
            columnPos = cursorPos - pos;
            break;
        }
        pos += lines[i].length() + 1; // +1 for newline
    }
    
    // Move to previous line
    if (currentLine > 0) {
        size_t newLineStart = 0;
        for (int i = 0; i < currentLine - 1; i++) {
            newLineStart += lines[i].length() + 1;
        }
        
        // Use preferred column or current column
        int targetColumn = (preferredColumn > 0) ? preferredColumn : columnPos;
        size_t newLineLength = lines[currentLine - 1].length();
        
        cursorPos = newLineStart + std::min(static_cast<size_t>(targetColumn), newLineLength);
        preferredColumn = targetColumn;
    }
}

void CodeEditorScreen::moveCursorDown() {
    auto lines = splitLines(code);
    if (lines.empty()) return;
    
    // Find current line and position within line
    size_t pos = 0;
    int currentLine = 0;
    int columnPos = 0;
    
    for (size_t i = 0; i < lines.size(); i++) {
        if (pos + lines[i].length() >= cursorPos) {
            currentLine = i;
            columnPos = cursorPos - pos;
            break;
        }
        pos += lines[i].length() + 1; // +1 for newline
    }
    
    // Move to next line
    if (currentLine < static_cast<int>(lines.size()) - 1) {
        size_t newLineStart = 0;
        for (int i = 0; i <= currentLine; i++) {
            newLineStart += lines[i].length() + 1;
        }
        
        // Use preferred column or current column
        int targetColumn = (preferredColumn > 0) ? preferredColumn : columnPos;
        size_t newLineLength = lines[currentLine + 1].length();
        
        cursorPos = newLineStart + std::min(static_cast<size_t>(targetColumn), newLineLength);
        preferredColumn = targetColumn;
    }
}

void CodeEditorScreen::drawSelection(sf::RenderWindow& window) {
    if (!hasSelection()) return;
    
    size_t start = std::min(selectionStart, selectionEnd);
    size_t end = std::max(selectionStart, selectionEnd);
    
    auto lines = splitLines(code);
    float lineHeight = font.getLineSpacing(14);
    size_t currentPos = 0;
    
    for (size_t i = 0; i < lines.size(); i++) {
        size_t lineLen = lines[i].length();
        size_t lineEndPos = currentPos + lineLen;
        
        // Check intersection with selection
        size_t selStartInLine = std::max(start, currentPos);
        size_t selEndInLine = std::min(end, lineEndPos);
        
        if (selStartInLine < selEndInLine) {
            // This line has selected text
            sf::Vector2f startPos = codeText.findCharacterPos(selStartInLine);
            sf::Vector2f endPos = codeText.findCharacterPos(selEndInLine);
            
            float width = endPos.x - startPos.x;
            
            sf::RectangleShape selRect({width, lineHeight});
            selRect.setPosition(startPos);
            selRect.setFillColor(sf::Color(100, 150, 255, 100));
            window.draw(selRect);
        }
        
        // Handle newline selection
        if (end > lineEndPos && start <= lineEndPos) {
            sf::Vector2f startPos = codeText.findCharacterPos(lineEndPos);
            sf::RectangleShape selRect({5.0f, lineHeight}); // 5px width indicator for newline
            selRect.setPosition(startPos);
            selRect.setFillColor(sf::Color(100, 150, 255, 100));
            window.draw(selRect);
        }
        
        currentPos += lineLen + 1; // +1 for newline
    }
}

AppState CodeEditorScreen::run(sf::RenderWindow& window) {
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                return AppState::EXIT;
            
            // Handle button clicks
            if (runBtn.isClicked(*event, window)) {
                executeCode();
            }
            
            if (clearBtn.isClicked(*event, window)) {
                code.clear();
                cursorPos = 0;
                output.clear();
                clearSelection();
                updateCodeDisplay();
                updateOutputDisplay();
            }
            
            if (logoutBtn.isClicked(*event, window)) {
                return AppState::LOGIN;
            }
            
            // Handle mouse clicks for cursor positioning
            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords({mouseEvent->position.x, mouseEvent->position.y});
                    size_t newPos = getCursorPosFromClick(mousePos);
                    
                    if (newPos != cursorPos) {
                        cursorPos = newPos;
                        clearSelection();
                        isSelecting = true;
                        updateCodeDisplay();
                    }
                }
            }
            
            // Handle mouse drag for selection
            if (const auto* mouseMoveEvent = event->getIf<sf::Event::MouseMoved>()) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && isSelecting) {
                    sf::Vector2f mousePos = window.mapPixelToCoords({mouseMoveEvent->position.x, mouseMoveEvent->position.y});
                    size_t newPos = getCursorPosFromClick(mousePos);
                    
                    if (newPos != cursorPos) {
                        selectionEnd = newPos;
                        cursorPos = newPos;
                        updateCodeDisplay();
                    }
                }
            }
            
            // Handle mouse release
            if (const auto* mouseReleaseEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseReleaseEvent->button == sf::Mouse::Button::Left) {
                    isSelecting = false;
                }
            }
            
            // Handle text input
            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                if (textEvent->unicode < 128) {
                    char c = static_cast<char>(textEvent->unicode);
                    
                    if (c == '\b') { // Backspace
                        if (hasSelection()) {
                            deleteSelection();
                            updateCodeDisplay();
                        } else if (cursorPos > 0) {
                            code.erase(cursorPos - 1, 1);
                            cursorPos--;
                            updateCodeDisplay();
                        }
                    } else if (c == '\r' || c == '\n') { // Enter
                        if (hasSelection()) {
                            deleteSelection();
                        }
                        code.insert(cursorPos, "\n");
                        cursorPos++;
                        clearSelection();
                        preferredColumn = 0;
                        updateCodeDisplay();
                    } else if (c >= 32) { // Printable characters
                        if (hasSelection()) {
                            deleteSelection();
                        }
                        code.insert(cursorPos, 1, c);
                        cursorPos++;
                        clearSelection();
                        updateCodeDisplay();
                    }
                }
            }
            
            // Handle special keys
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                bool shiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || 
                                   sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
                bool ctrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || 
                                  sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
                
                // Clipboard operations
                if (ctrlPressed) {
                    if (keyEvent->code == sf::Keyboard::Key::C) {
                        // Copy
                        if (hasSelection()) {
                            sf::Clipboard::setString(getSelectedText());
                        }
                    } else if (keyEvent->code == sf::Keyboard::Key::V) {
                        // Paste
                        if (hasSelection()) {
                            deleteSelection();
                        }
                        std::string clipboardText = sf::Clipboard::getString();
                        code.insert(cursorPos, clipboardText);
                        cursorPos += clipboardText.length();
                        clearSelection();
                        updateCodeDisplay();
                    } else if (keyEvent->code == sf::Keyboard::Key::X) {
                        // Cut
                        if (hasSelection()) {
                            sf::Clipboard::setString(getSelectedText());
                            deleteSelection();
                            updateCodeDisplay();
                        }
                    } else if (keyEvent->code == sf::Keyboard::Key::A) {
                        // Select all
                        selectionStart = 0;
                        selectionEnd = code.length();
                        cursorPos = code.length();
                        updateCodeDisplay();
                    }
                }
                
                // Arrow key navigation
                if (keyEvent->code == sf::Keyboard::Key::Left && cursorPos > 0) {
                    if (shiftPressed) {
                        if (!hasSelection()) {
                            selectionStart = cursorPos;
                        }
                        cursorPos--;
                        selectionEnd = cursorPos;
                    } else {
                        cursorPos--;
                        clearSelection();
                    }
                    preferredColumn = 0;
                    updateCodeDisplay();
                } else if (keyEvent->code == sf::Keyboard::Key::Right && cursorPos < code.length()) {
                    if (shiftPressed) {
                        if (!hasSelection()) {
                            selectionStart = cursorPos;
                        }
                        cursorPos++;
                        selectionEnd = cursorPos;
                    } else {
                        cursorPos++;
                        clearSelection();
                    }
                    preferredColumn = 0;
                    updateCodeDisplay();
                } else if (keyEvent->code == sf::Keyboard::Key::Up) {
                    if (shiftPressed) {
                        if (!hasSelection()) {
                            selectionStart = cursorPos;
                        }
                        moveCursorUp();
                        selectionEnd = cursorPos;
                    } else {
                        moveCursorUp();
                        clearSelection();
                    }
                    updateCodeDisplay();
                } else if (keyEvent->code == sf::Keyboard::Key::Down) {
                    if (shiftPressed) {
                        if (!hasSelection()) {
                            selectionStart = cursorPos;
                        }
                        moveCursorDown();
                        selectionEnd = cursorPos;
                    } else {
                        moveCursorDown();
                        clearSelection();
                    }
                    updateCodeDisplay();
                } else if (keyEvent->code == sf::Keyboard::Key::Home) {
                    // Move to start of line
                    if (shiftPressed && !hasSelection()) {
                        selectionStart = cursorPos;
                    }
                    while (cursorPos > 0 && code[cursorPos - 1] != '\n') {
                        cursorPos--;
                    }
                    if (shiftPressed) {
                        selectionEnd = cursorPos;
                    } else {
                        clearSelection();
                    }
                    preferredColumn = 0;
                    updateCodeDisplay();
                } else if (keyEvent->code == sf::Keyboard::Key::End) {
                    // Move to end of line
                    if (shiftPressed && !hasSelection()) {
                        selectionStart = cursorPos;
                    }
                    while (cursorPos < code.length() && code[cursorPos] != '\n') {
                        cursorPos++;
                    }
                    if (shiftPressed) {
                        selectionEnd = cursorPos;
                    } else {
                        clearSelection();
                    }
                    preferredColumn = 0;
                    updateCodeDisplay();
                } else if (keyEvent->code == sf::Keyboard::Key::Tab) {
                    // Insert 4 spaces for tab
                    if (hasSelection()) {
                        deleteSelection();
                    }
                    code.insert(cursorPos, "    ");
                    cursorPos += 4;
                    clearSelection();
                    updateCodeDisplay();
                } else if (keyEvent->code == sf::Keyboard::Key::Delete) {
                    // Delete key
                    if (hasSelection()) {
                        deleteSelection();
                    } else if (cursorPos < code.length()) {
                        code.erase(cursorPos, 1);
                    }
                    updateCodeDisplay();
                }
            }
        }
        
        // Update buttons
        runBtn.update(window);
        clearBtn.update(window);
        logoutBtn.update(window);
        particles.update();
        
        // Draw
        window.clear();
        
        // Draw gradient background
        sf::VertexArray gradient(sf::PrimitiveType::TriangleStrip, 4);
        gradient[0].position = {0, 0}; gradient[0].color = sf::Color(15, 23, 42);
        gradient[1].position = {0, 600}; gradient[1].color = sf::Color(30, 41, 59);
        gradient[2].position = {800, 0}; gradient[2].color = sf::Color(15, 23, 42);
        gradient[3].position = {800, 600}; gradient[3].color = sf::Color(51, 65, 85);
        window.draw(gradient);
        
        particles.draw(window);
        
        // Draw UI
        window.draw(editorBox);
        window.draw(outputBox);
        window.draw(editorLabel);
        window.draw(outputLabel);
        
        // Draw selection before text
        drawSelection(window);
        
        window.draw(codeText);
        window.draw(outputText);
        
        // Draw blinking cursor
        if (cursorBlink.getElapsedTime().asSeconds() < 0.5f) {
            window.draw(cursor);
        }
        if (cursorBlink.getElapsedTime().asSeconds() >= 1.0f) {
            cursorBlink.restart();
        }
        
        runBtn.draw(window);
        clearBtn.draw(window);
        logoutBtn.draw(window);
        
        window.display();
    }
    
    return AppState::EXIT;
}
