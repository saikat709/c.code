#include "CodeEditorScreen.hpp"
#include <SFML/Graphics.hpp>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <array>
#include <memory>
#include "UI.hpp"

using namespace std;
using namespace sf;


CodeEditorScreen::CodeEditorScreen(Font& font, ParticleSystem& particles, Vector2u windowSize)
    : font(font), particles(particles),
      newFileBtn(font, "+ New File", {10, 560}, {180, 35}),
      fileMenuBtn(font, "File", {205, 5}, {60, 30}),
      editMenuBtn(font, "Edit", {270, 5}, {60, 30}),
      viewMenuBtn(font, "View", {335, 5}, {60, 30}),
      runMenuBtn(font, "Run", {400, 5}, {60, 30}),
      toggleOutputBtn(font, "Output", {465, 5}, {80, 30}),
      logoutBtn(font, "Logout", {700, 5}, {90, 30}),
      editorLabel(font, "Code Editor", 18),
      outputLabel(font, "Output", 18),
      codeText(font, "", 14),
      outputText(font, "", 14),
      sidebarTitle(font, "FILES", 14)
{
    // Sidebar Box (left side, full height minus top bar)
    sidebarBox.setSize({200, 560});
    sidebarBox.setPosition({0, 40});
    sidebarBox.setFillColor(Color(20, 30, 45, 240));
    sidebarBox.setOutlineThickness(1);
    sidebarBox.setOutlineColor(Color(60, 80, 120));
    
    // Sidebar Title
    sidebarTitle.setFillColor(Color(150, 170, 200));
    sidebarTitle.setPosition({10, 45});
    
    // File naming input box
    fileNameInputBox.setSize({180, 25});
    fileNameInputBox.setPosition({10, 70});
    fileNameInputBox.setFillColor(Color(40, 50, 70));
    fileNameInputBox.setOutlineThickness(2);
    fileNameInputBox.setOutlineColor(Color(100, 150, 200));
    
    // File naming input text
    fileNameInputText = Text(font, "", 12);
    fileNameInputText.setFillColor(Color::White);
    fileNameInputText.setPosition({15, 73});
    
    // Top Menu Bar
    topMenuBar.setSize({800, 40});
    topMenuBar.setPosition({0, 0});
    topMenuBar.setFillColor(Color(25, 35, 50, 250));
    topMenuBar.setOutlineThickness(1);
    topMenuBar.setOutlineColor(Color(60, 80, 120));
    
    // Editor Box (right side, adjusted for sidebar and top bar)
    editorBox.setSize({600, 560});
    editorBox.setPosition({200, 40});
    editorBox.setFillColor(Color(30, 41, 59, 220));
    editorBox.setOutlineThickness(2);
    editorBox.setOutlineColor(Color(100, 150, 200));
    
    // Output Box (initially hidden, will be shown when toggled)
    outputBox.setSize({600, 0});
    outputBox.setPosition({200, 600});
    outputBox.setFillColor(Color(20, 30, 45, 220));
    outputBox.setOutlineThickness(2);
    outputBox.setOutlineColor(Color(150, 100, 100));
    
    
    // Labels
    editorLabel.setFillColor(Color::White);
    editorLabel.setPosition({205, 45});
    
    outputLabel.setFillColor(Color::White);
    outputLabel.setPosition({205, 345});
    
    // Code Text
    codeText.setFillColor(Color(220, 220, 220));
    codeText.setPosition({210, 70});
    
    // Output Text
    outputText.setFillColor(Color(200, 255, 200));
    outputText.setPosition({210, 370});
    
    // Cursor
    cursor.setSize({2, 16});
    cursor.setFillColor(Color::White);
    
    // Initialize file list with sample files
    fileList.push_back("main.cpp");
    fileList.push_back("temp.cpp");
    fileList.push_back("utils.cpp");
    
    // Default code template
    code = "#include <iostream>\nusing namespace std;\n\nint main() {\n    cout << \"Hello, World!\" << endl;\n    return 0;\n}\n";
    cursorPos = code.length();
    updateCodeDisplay();
}

vector<string> CodeEditorScreen::splitLines(const string& text) {
    vector<string> lines;
    stringstream ss(text);
    string line;
    while (getline(ss, line)) {
        lines.push_back(line);
    }
    return lines;
}

void CodeEditorScreen::updateCodeDisplay() {
    codeText.setString(code);
    
    // Update cursor position using SFML's text metrics
    Vector2f pos = codeText.findCharacterPos(cursorPos);
    cursor.setPosition(pos);
}

void CodeEditorScreen::updateOutputDisplay() {
    outputText.setString(output);
}

void CodeEditorScreen::executeCode() {
    output = "Compiling...\n";
    updateOutputDisplay();
    
    // Write code to temporary file
    ofstream tempFile("temp_code.cpp");
    if (!tempFile) {
        output = "Error: Could not create temporary file\n";
        updateOutputDisplay();
        return;
    }
    tempFile << code;
    tempFile.close();
    
    // Compile the code
    string compileCmd = "g++ -std=c++17 temp_code.cpp -o temp_program 2>&1";
    
    array<char, 128> buffer;
    string compileOutput;
    
    #ifdef _WIN32
    unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(compileCmd.c_str(), "r"), _pclose);
    #else
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(compileCmd.c_str(), "r"), pclose);
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
    string runCmd = "temp_program.exe 2>&1";
    unique_ptr<FILE, decltype(&_pclose)> runPipe(_popen(runCmd.c_str(), "r"), _pclose);
    #else
    string runCmd = "./temp_program 2>&1";
    unique_ptr<FILE, decltype(&pclose)> runPipe(popen(runCmd.c_str(), "r"), pclose);
    #endif
    
    if (!runPipe) {
        output = "Error: Could not execute program\n";
        updateOutputDisplay();
        return;
    }
    
    string programOutput;
    while (fgets(buffer.data(), buffer.size(), runPipe.get()) != nullptr) {
        programOutput += buffer.data();
    }
    
    output = "Output:\n" + programOutput;
    if (output == "Output:\n") {
        output = "Output:\n(Program executed successfully with no output)";
    }
    updateOutputDisplay();
    
    // Cleanup
    remove("temp_code.cpp");
    #ifdef _WIN32
    remove("temp_program.exe");
    #else
    remove("temp_program");
    #endif
}

// New helper methods to add after line 192 in CodeEditorScreen.cpp

void CodeEditorScreen::updateLayout() {
    if (outputVisible) {
        editorBox.setSize({600, 280});
        editorBox.setPosition({200, 40});
        outputBox.setSize({600, 280});
        outputBox.setPosition({200, 320});
        editorLabel.setPosition({205, 45});
        outputLabel.setPosition({205, 325});
        codeText.setPosition({210, 70});
        outputText.setPosition({210, 350});
    } else {
        editorBox.setSize({600, 560});
        editorBox.setPosition({200, 40});
        outputBox.setSize({600, 0});
        outputBox.setPosition({200, 600});
        editorLabel.setPosition({205, 45});
        codeText.setPosition({210, 70});
    }
    updateCodeDisplay();
    updateOutputDisplay();
}

void CodeEditorScreen::toggleOutputPanel() {
    outputVisible = !outputVisible;
    updateLayout();
}

void CodeEditorScreen::createNewFile() {
    // Activate file naming mode
    isNamingFile = true;
    newFileName = "";
    fileNameInputText.setString("");
}

void CodeEditorScreen::handleFileSelection(Vector2f mousePos) {
    // Determine file list start position based on naming mode
    float fileListStartY = isNamingFile ? 100.0f : 70.0f;
    
    if (mousePos.x < 0 || mousePos.x > 200 || mousePos.y < fileListStartY || mousePos.y > 560) {
        return;
    }
    float lineHeight = 25.0f;
    int clickedIndex = static_cast<int>((mousePos.y - fileListStartY) / lineHeight);
    if (clickedIndex >= 0 && clickedIndex < static_cast<int>(fileList.size())) {
        selectedFileIndex = clickedIndex;
        editorLabel.setString("Code Editor - " + fileList[selectedFileIndex]);
    }
}

void CodeEditorScreen::drawSidebar(RenderWindow& window) {
    window.draw(sidebarBox);
    window.draw(sidebarTitle);
    
    // Draw file naming input if active
    float fileListStartY = 70;
    if (isNamingFile) {
        window.draw(fileNameInputBox);
        window.draw(fileNameInputText);
        fileListStartY = 100;  // Start file list below input
    }
    
    // Draw file list
    float yPos = fileListStartY;
    float lineHeight = 25.0f;
    for (size_t i = 0; i < fileList.size(); i++) {
        if (static_cast<int>(i) == selectedFileIndex) {
            RectangleShape highlight({195, lineHeight - 2});
            highlight.setPosition({2.5f, yPos});
            highlight.setFillColor(Color(60, 80, 120, 150));
            window.draw(highlight);
        }
        Text fileText(font, fileList[i], 12);
        fileText.setPosition({10, yPos + 3});
        fileText.setFillColor(Color(200, 220, 240));
        window.draw(fileText);
        yPos += lineHeight;
    }
    
    // Draw new file button at bottom
    newFileBtn.draw(window);
}

void CodeEditorScreen::drawTopMenu(RenderWindow& window) {
    window.draw(topMenuBar);
    fileMenuBtn.draw(window);
    editMenuBtn.draw(window);
    viewMenuBtn.draw(window);
    runMenuBtn.draw(window);
    toggleOutputBtn.draw(window);
    logoutBtn.draw(window);
}

bool CodeEditorScreen::hasSelection() {
    return selectionStart != selectionEnd;
}

void CodeEditorScreen::clearSelection() {
    selectionStart = selectionEnd = cursorPos;
}

string CodeEditorScreen::getSelectedText() {
    if (!hasSelection()) return "";
    size_t start = min(selectionStart, selectionEnd);
    size_t end = max(selectionStart, selectionEnd);
    return code.substr(start, end - start);
}

void CodeEditorScreen::deleteSelection() {
    if (!hasSelection()) return;
    size_t start = min(selectionStart, selectionEnd);
    size_t end = max(selectionStart, selectionEnd);
    code.erase(start, end - start);
    cursorPos = start;
    clearSelection();
}

size_t CodeEditorScreen::getCursorPosFromClick(Vector2f mousePos) {
    // Check if click is within editor bounds (adjusted for new layout)
    float editorHeight = outputVisible ? 280.0f : 560.0f;
    if (mousePos.x < 200 || mousePos.x > 800 || mousePos.y < 70 || mousePos.y > (40 + editorHeight)) {
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
    string& line = lines[lineIndex];
    float bestDist = 100000.0f;
    size_t bestOffset = 0;
    
    // Check all character positions including the one after the last char
    for (size_t i = 0; i <= line.length(); ++i) {
        Vector2f charPos = codeText.findCharacterPos(currentPos + i);
        float dist = abs(mousePos.x - charPos.x);
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
        
        cursorPos = newLineStart + min(static_cast<size_t>(targetColumn), newLineLength);
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
        
        cursorPos = newLineStart + min(static_cast<size_t>(targetColumn), newLineLength);
        preferredColumn = targetColumn;
    }
}

void CodeEditorScreen::drawSelection(RenderWindow& window) {
    if (!hasSelection()) return;
    
    size_t start = min(selectionStart, selectionEnd);
    size_t end = max(selectionStart, selectionEnd);
    
    auto lines = splitLines(code);
    float lineHeight = font.getLineSpacing(14);
    size_t currentPos = 0;
    
    for (size_t i = 0; i < lines.size(); i++) {
        size_t lineLen = lines[i].length();
        size_t lineEndPos = currentPos + lineLen;
        
        // Check intersection with selection
        size_t selStartInLine = max(start, currentPos);
        size_t selEndInLine = min(end, lineEndPos);
        
        if (selStartInLine < selEndInLine) {
            // This line has selected text
            Vector2f startPos = codeText.findCharacterPos(selStartInLine);
            Vector2f endPos = codeText.findCharacterPos(selEndInLine);
            
            float width = endPos.x - startPos.x;
            
            RectangleShape selRect({width, lineHeight});
            selRect.setPosition(startPos);
            selRect.setFillColor(Color(100, 150, 255, 100));
            window.draw(selRect);
        }
        
        // Handle newline selection
        if (end > lineEndPos && start <= lineEndPos) {
            Vector2f startPos = codeText.findCharacterPos(lineEndPos);
            RectangleShape selRect({5.0f, lineHeight}); // 5px width indicator for newline
            selRect.setPosition(startPos);
            selRect.setFillColor(Color(100, 150, 255, 100));
            window.draw(selRect);
        }
        
        currentPos += lineLen + 1; // +1 for newline
    }
}

AppState CodeEditorScreen::run(RenderWindow& window) {
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                return AppState::EXIT;
            
            // Handle button clicks
            if (runMenuBtn.isClicked(*event, window)) {
                if (!outputVisible) {
                    toggleOutputPanel();
                }
                executeCode();
            }
            
            if (toggleOutputBtn.isClicked(*event, window)) {
                toggleOutputPanel();
            }
            
            if (newFileBtn.isClicked(*event, window)) {
                createNewFile();
            }
            
            if (logoutBtn.isClicked(*event, window)) {
                return AppState::LOGIN;
            }
            
            // Handle mouse clicks for cursor positioning and file selection
            if (const auto* mouseEvent = event->getIf<Event::MouseButtonPressed>()) {
                if (mouseEvent->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords({mouseEvent->position.x, mouseEvent->position.y});
                    
                    // Check if click is in sidebar
                    if (mousePos.x >= 0 && mousePos.x <= 200 && mousePos.y >= 90) {
                        handleFileSelection(mousePos);
                    }
                    // Check if click is in editor area
                    else if (mousePos.x >= 200 && mousePos.x <= 800 && mousePos.y >= 40) {
                        size_t newPos = getCursorPosFromClick(mousePos);
                        
                        if (newPos != cursorPos) {
                            cursorPos = newPos;
                            clearSelection();
                            isSelecting = true;
                            updateCodeDisplay();
                        }
                    }
                }
            }
            
            // Handle mouse drag for selection
            if (const auto* mouseMoveEvent = event->getIf<Event::MouseMoved>()) {
                if (Mouse::isButtonPressed(Mouse::Button::Left) && isSelecting) {
                    Vector2f mousePos = window.mapPixelToCoords({mouseMoveEvent->position.x, mouseMoveEvent->position.y});
                    size_t newPos = getCursorPosFromClick(mousePos);
                    
                    if (newPos != cursorPos) {
                        selectionEnd = newPos;
                        cursorPos = newPos;
                        updateCodeDisplay();
                    }
                }
            }
            
            // Handle mouse release
            if (const auto* mouseReleaseEvent = event->getIf<Event::MouseButtonReleased>()) {
                if (mouseReleaseEvent->button == Mouse::Button::Left) {
                    isSelecting = false;
                }
            }
            
            // Handle text input
            if (const auto* textEvent = event->getIf<Event::TextEntered>()) {
                if (textEvent->unicode < 128) {
                    char c = static_cast<char>(textEvent->unicode);
                    
                    // Handle file naming input
                    if (isNamingFile) {
                        if (c == '\r' || c == '\n') { // Enter - confirm file name
                            if (!newFileName.empty()) {
                                // Add .cpp extension if not present
                                if (newFileName.find(".") == string::npos) {
                                    newFileName += ".cpp";
                                }
                                fileList.push_back(newFileName);
                                selectedFileIndex = fileList.size() - 1;
                                code = "// " + newFileName + "\n\n";
                                cursorPos = code.length();
                                output.clear();
                                clearSelection();
                                updateCodeDisplay();
                                updateOutputDisplay();
                                editorLabel.setString("Code Editor - " + newFileName);
                            }
                            isNamingFile = false;
                            newFileName = "";
                        } else if (c == 27) { // Escape - cancel
                            isNamingFile = false;
                            newFileName = "";
                        } else if (c == '\b') { // Backspace
                            if (!newFileName.empty()) {
                                newFileName.pop_back();
                                fileNameInputText.setString(newFileName);
                            }
                        } else if (c >= 32 && c < 127) { // Printable characters
                            newFileName += c;
                            fileNameInputText.setString(newFileName);
                        }
                        continue; // Skip regular text input handling
                    }
                    
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
            if (const auto* keyEvent = event->getIf<Event::KeyPressed>()) {
                bool shiftPressed = Keyboard::isKeyPressed(Keyboard::Key::LShift) || 
                                   Keyboard::isKeyPressed(Keyboard::Key::RShift);
                bool ctrlPressed = Keyboard::isKeyPressed(Keyboard::Key::LControl) || 
                                  Keyboard::isKeyPressed(Keyboard::Key::RControl);
                
                // Clipboard operations
                if (ctrlPressed) {
                    if (keyEvent->code == Keyboard::Key::C) {
                        // Copy
                        if (hasSelection()) {
                            Clipboard::setString(getSelectedText());
                        }
                    } else if (keyEvent->code == Keyboard::Key::V) {
                        // Paste
                        if (hasSelection()) {
                            deleteSelection();
                        }
                        string clipboardText = Clipboard::getString();
                        code.insert(cursorPos, clipboardText);
                        cursorPos += clipboardText.length();
                        clearSelection();
                        updateCodeDisplay();
                    } else if (keyEvent->code == Keyboard::Key::X) {
                        // Cut
                        if (hasSelection()) {
                            Clipboard::setString(getSelectedText());
                            deleteSelection();
                            updateCodeDisplay();
                        }
                    } else if (keyEvent->code == Keyboard::Key::A) {
                        // Select all
                        selectionStart = 0;
                        selectionEnd = code.length();
                        cursorPos = code.length();
                        updateCodeDisplay();
                    }
                }
                
                // Arrow key navigation
                if (keyEvent->code == Keyboard::Key::Left && cursorPos > 0) {
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
                } else if (keyEvent->code == Keyboard::Key::Right && cursorPos < code.length()) {
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
                } else if (keyEvent->code == Keyboard::Key::Up) {
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
                } else if (keyEvent->code == Keyboard::Key::Down) {
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
                } else if (keyEvent->code == Keyboard::Key::Home) {
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
                } else if (keyEvent->code == Keyboard::Key::End) {
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
                } else if (keyEvent->code == Keyboard::Key::Tab) {
                    // Insert 4 spaces for tab
                    if (hasSelection()) {
                        deleteSelection();
                    }
                    code.insert(cursorPos, "    ");
                    cursorPos += 4;
                    clearSelection();
                    updateCodeDisplay();
                } else if (keyEvent->code == Keyboard::Key::Delete) {
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
        newFileBtn.update(window);
        fileMenuBtn.update(window);
        editMenuBtn.update(window);
        viewMenuBtn.update(window);
        runMenuBtn.update(window);
        toggleOutputBtn.update(window);
        logoutBtn.update(window);
        particles.update();
        
        // Draw
        window.clear();
        
        // Draw gradient background
        VertexArray gradient(PrimitiveType::TriangleStrip, 4);
        gradient[0].position = {0, 0}; gradient[0].color = Color(15, 23, 42);
        gradient[1].position = {0, 600}; gradient[1].color = Color(30, 41, 59);
        gradient[2].position = {800, 0}; gradient[2].color = Color(15, 23, 42);
        gradient[3].position = {800, 600}; gradient[3].color = Color(51, 65, 85);
        window.draw(gradient);
        
        particles.draw(window);
        
        // Draw UI
        drawTopMenu(window);
        drawSidebar(window);
        
        window.draw(editorBox);
        if (outputVisible) {
            window.draw(outputBox);
            window.draw(outputLabel);
            window.draw(outputText);
        }
        window.draw(editorLabel);
        
        // Draw selection before text
        drawSelection(window);
        
        window.draw(codeText);
        // Draw blinking cursor
        if (cursorBlink.getElapsedTime().asSeconds() < 0.5f) {
            window.draw(cursor);
        }
        if (cursorBlink.getElapsedTime().asSeconds() >= 1.0f) {
            cursorBlink.restart();
        }
        
        window.display();
    }
    
    return AppState::EXIT;
}
