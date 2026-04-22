#include "CodeEditorScreen.hpp"

#include <sstream>

using namespace std;
using namespace sf;

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
    if (cursorPos > code.length()) {
        cursorPos = code.length();
    }
    codeText.setString(code);
    Vector2f pos = codeText.findCharacterPos(cursorPos);
    cursor.setPosition(pos);
}

void CodeEditorScreen::updateOutputDisplay() {
    outputText.setString(output);
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
    float topBarHeight = 40.0f;
    float availableHeight = windowHeight - topBarHeight;
    float editorHeight = outputVisible ? (availableHeight - outputHeight) : availableHeight;
    float editorWidth = windowWidth - sidebarWidth - (chatVisible ? chatWidth : 0);

    if (mousePos.x < sidebarWidth || mousePos.x > (sidebarWidth + editorWidth) ||
        mousePos.y < (topBarHeight + 30) || mousePos.y > (topBarHeight + editorHeight)) {
        return cursorPos;
    }

    auto lines = splitLines(code);
    if (lines.empty()) {
        return 0;
    }

    float lineHeight = font.getLineSpacing(14);
    float relativeY = mousePos.y - codeText.getPosition().y;
    int lineIndex = static_cast<int>(relativeY / lineHeight);

    if (lineIndex < 0) lineIndex = 0;
    if (lineIndex >= static_cast<int>(lines.size())) lineIndex = static_cast<int>(lines.size()) - 1;

    size_t currentPos = 0;
    for (int i = 0; i < lineIndex; ++i) {
        currentPos += lines[i].length() + 1;
    }

    const string& line = lines[lineIndex];
    float bestDist = 100000.0f;
    size_t bestOffset = 0;

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

    size_t pos = 0;
    int currentLine = 0;
    int columnPos = 0;
    for (size_t i = 0; i < lines.size(); i++) {
        if (pos + lines[i].length() >= cursorPos) {
            currentLine = static_cast<int>(i);
            columnPos = static_cast<int>(cursorPos - pos);
            break;
        }
        pos += lines[i].length() + 1;
    }

    if (currentLine > 0) {
        size_t newLineStart = 0;
        for (int i = 0; i < currentLine - 1; i++) newLineStart += lines[i].length() + 1;
        int targetColumn = (preferredColumn > 0) ? preferredColumn : columnPos;
        size_t newLineLength = lines[currentLine - 1].length();
        cursorPos = newLineStart + min(static_cast<size_t>(targetColumn), newLineLength);
        preferredColumn = targetColumn;
    }
}

void CodeEditorScreen::moveCursorDown() {
    auto lines = splitLines(code);
    if (lines.empty()) return;

    size_t pos = 0;
    int currentLine = 0;
    int columnPos = 0;
    for (size_t i = 0; i < lines.size(); i++) {
        if (pos + lines[i].length() >= cursorPos) {
            currentLine = static_cast<int>(i);
            columnPos = static_cast<int>(cursorPos - pos);
            break;
        }
        pos += lines[i].length() + 1;
    }

    if (currentLine < static_cast<int>(lines.size()) - 1) {
        size_t newLineStart = 0;
        for (int i = 0; i <= currentLine; i++) newLineStart += lines[i].length() + 1;
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
        size_t selStartInLine = max(start, currentPos);
        size_t selEndInLine = min(end, lineEndPos);
        if (selStartInLine < selEndInLine) {
            Vector2f startPos = codeText.findCharacterPos(selStartInLine);
            Vector2f endPos = codeText.findCharacterPos(selEndInLine);
            float width = endPos.x - startPos.x;
            RectangleShape selRect({width, lineHeight});
            selRect.setPosition(startPos);
            selRect.setFillColor(Color(100, 150, 255, 100));
            window.draw(selRect);
        }
        if (end > lineEndPos && start <= lineEndPos) {
            Vector2f startPos = codeText.findCharacterPos(lineEndPos);
            RectangleShape selRect({5.0f, lineHeight});
            selRect.setPosition(startPos);
            selRect.setFillColor(Color(100, 150, 255, 100));
            window.draw(selRect);
        }
        currentPos += lineLen + 1;
    }
}