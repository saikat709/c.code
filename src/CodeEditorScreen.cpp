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
    
    // Update cursor position
    std::string beforeCursor = code.substr(0, cursorPos);
    auto lines = splitLines(beforeCursor);
    int lineNum = lines.size() - 1;
    std::string currentLine = lines.empty() ? "" : lines.back();
    
    sf::Text tempText(font, currentLine, 14);
    
    float cursorX = 30 + tempText.getLocalBounds().size.x;
    float cursorY = 60 + lineNum * 18;
    
    cursor.setPosition({cursorX, cursorY});
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
                updateCodeDisplay();
                updateOutputDisplay();
            }
            
            if (logoutBtn.isClicked(*event, window)) {
                return AppState::LOGIN;
            }
            
            // Handle text input
            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                if (textEvent->unicode < 128) {
                    char c = static_cast<char>(textEvent->unicode);
                    
                    if (c == '\b') { // Backspace
                        if (cursorPos > 0) {
                            code.erase(cursorPos - 1, 1);
                            cursorPos--;
                            updateCodeDisplay();
                        }
                    } else if (c == '\r' || c == '\n') { // Enter
                        code.insert(cursorPos, "\n");
                        cursorPos++;
                        updateCodeDisplay();
                    } else if (c >= 32) { // Printable characters
                        code.insert(cursorPos, 1, c);
                        cursorPos++;
                        updateCodeDisplay();
                    }
                }
            }
            
            // Handle special keys
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Left && cursorPos > 0) {
                    cursorPos--;
                    updateCodeDisplay();
                } else if (keyEvent->code == sf::Keyboard::Key::Right && cursorPos < code.length()) {
                    cursorPos++;
                    updateCodeDisplay();
                } else if (keyEvent->code == sf::Keyboard::Key::Home) {
                    // Move to start of line
                    while (cursorPos > 0 && code[cursorPos - 1] != '\n') {
                        cursorPos--;
                    }
                    updateCodeDisplay();
                } else if (keyEvent->code == sf::Keyboard::Key::End) {
                    // Move to end of line
                    while (cursorPos < code.length() && code[cursorPos] != '\n') {
                        cursorPos++;
                    }
                    updateCodeDisplay();
                } else if (keyEvent->code == sf::Keyboard::Key::Tab) {
                    // Insert 4 spaces for tab
                    code.insert(cursorPos, "    ");
                    cursorPos += 4;
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
