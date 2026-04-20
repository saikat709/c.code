#include <fstream>
#include <sstream>
#include "CodeEditorScreen.hpp"
#include "UI.hpp"
#include "Session.hpp"
#include "json.hpp"

using namespace std;
using namespace sf;


CodeEditorScreen::CodeEditorScreen(Font& font, ParticleSystem& particles, Vector2u windowSize)
    : font(font), particles(particles),
      windowWidth((float)windowSize.x), windowHeight((float)windowSize.y),
      newFileBtn(font, "+ New File", {10, 560}, {180, 35}),
      fileMenuBtn(font, "File", {20, 5}, {60, 30}),
      editMenuBtn(font, "Edit", {85, 5}, {60, 30}),
      viewMenuBtn(font, "View", {150, 5}, {60, 30}),
      runMenuBtn(font, "Run", {215, 5}, {60, 30}),
      toggleOutputBtn(font, "Output", {280, 5}, {80, 30}),
      chatBtn(font, "Chat", {365, 5}, {80, 30}),
      shareBtn(font, "Share", {450, 5}, {80, 30}),
      backBtn(font, "Back", {535, 5}, {80, 30}),
      logoutBtn(font, "Logout", {700, 5}, {90, 30}),
      editorLabel("Code Editor", font, 18),
      outputLabel("Output", font, 18),
      codeText("", font, 14),
      outputText("", font, 14),
      sidebarTitle("FILES", font, 14),
      fileNameInputText("", font, 12),
      chatInputField(font, "Type a message...", {windowSize.x - 340.0f, windowSize.y - 50.0f}, {250, 40}),
      sendChatBtn(font, "Send", {windowSize.x - 80.0f, windowSize.y - 50.0f}, {70, 40})
{
    // Chat Box
    chatBox.setSize({350, 560});
    chatBox.setPosition({450, 40}); // Adjust position to be a "second screen"
    chatBox.setFillColor(Color(15, 23, 42, 250));
    chatBox.setOutlineThickness(1);
    chatBox.setOutlineColor(Color(100, 150, 255, 100));

    // Chat Title
    chatTitle.setFont(font);
    chatTitle.setString("CHAT");
    chatTitle.setCharacterSize(16);
    chatTitle.setFillColor(Color(150, 170, 200));
    chatTitle.setPosition({460, 45});


    // Share Popup
    sharePopupCard.setSize({300, 200});
    sharePopupCard.setFillColor(Color(30, 41, 59));
    sharePopupCard.setOutlineThickness(2);
    sharePopupCard.setOutlineColor(Color(100, 150, 255));
    
    sharePopupTitle.setFont(font);
    sharePopupTitle.setCharacterSize(20);
    sharePopupTitle.setFillColor(Color::White);
    sharePopupTitle.setString("Project Access Info");
    
    sharePopupInfo.setFont(font);
    sharePopupInfo.setCharacterSize(16);
    sharePopupInfo.setFillColor(Color::Cyan);

    // Edit request dialog setup
    editRequestCard.setSize({400, 250});
    editRequestCard.setFillColor(Color(30, 41, 59));
    editRequestCard.setOutlineThickness(2);
    editRequestCard.setOutlineColor(Color(100, 150, 255));

    editRequestTitle.setFont(font);
    editRequestTitle.setCharacterSize(20);
    editRequestTitle.setFillColor(Color::White);
    editRequestTitle.setString("Edit Request");

    editRequestMessage.setFont(font);
    editRequestMessage.setCharacterSize(16);
    editRequestMessage.setFillColor(Color(200, 220, 255));

    allowEditBtn = Button(font, "Allow", {0, 0}, {120, 40});
    denyEditBtn = Button(font, "Deny", {0, 0}, {120, 40});

    
    popupOverlay.setSize({800, 600});
    popupOverlay.setFillColor(Color(0, 0, 0, 150));
    popupOverlay.setPosition({0, 0});
    
    closeShareBtn = Button(font, "OK", {0, 0}, {80, 30}); // Will position in layout
    showSharePopup = false;
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
    fileNameInputText = Text("", font, 12);
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
    
    float availableHeight = windowSize.y - 40.0f;
    // Resize Handles
    sidebarHandle.setSize({5, availableHeight});
    sidebarHandle.setFillColor(Color(100, 150, 255, 100));
    
    chatHandle.setSize({5, availableHeight});
    chatHandle.setFillColor(Color(100, 150, 255, 100));
    
    outputHandle.setSize({800, 5});
    outputHandle.setFillColor(Color(100, 150, 255, 100));

    updateLayout();
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

void CodeEditorScreen::loadFiles() {
    json request;
    request["action"] = "get_files";
    request["project_id"] = Session::getInstance().getCurrentProjectId();
    
    json response = Session::getInstance().getNetworkClient()->sendRequest(request);
    if (response["status"] == "success") {
        fileList.clear();
        fileIds.clear();
        fileLockedStatus.clear();
        fileLockedBy.clear();
        
        for (auto& fileJson : response["files"]) {
            fileList.push_back(fileJson["name"]);
            fileIds.push_back(fileJson["id"]);
            fileLockedStatus.push_back(fileJson.value("locked", false));
            fileLockedBy.push_back(fileJson.value("lockedBy", ""));
        }
        
        if (currentFileId == -1 && !fileIds.empty()) {
            selectedFileIndex = 0;
            requestFileEdit(fileIds[0]); // Request lock for first file
            fetchFileContent(fileIds[0]);
        }
    }
}

void CodeEditorScreen::pollForUpdates() {
    if (pollClock.getElapsedTime().asSeconds() < 2.0f) return;
    pollClock.restart();
    
    // Poll for new messages
    json request;
    request["action"] = "get_messages_since";
    request["project_id"] = Session::getInstance().getCurrentProjectId();
    request["last_message_id"] = lastMessageId;
    
    json response = Session::getInstance().getNetworkClient()->sendRequest(request);
    
    if (response["status"] == "success" && !response["messages"].empty()) {
        bool newMessages = false;
        for (auto& msg : response["messages"]) {
            int msgId = msg["id"];
            if (msgId > lastMessageId) {
                lastMessageId = msgId;
                newMessages = true;
            }
        }
        if (newMessages) loadMessages(); 
    }
    
    // Simple polling for file locks
    static sf::Clock filePollClock;
    if (filePollClock.getElapsedTime().asSeconds() > 5.0f) {
         loadFiles();
         filePollClock.restart();
    }
    
    // Process any asynchronous notifications received during the above requests
    auto notifications = Session::getInstance().getNetworkClient()->getPendingNotifications();
    for (const auto& notif : notifications) {
         handleServerBroadcast(notif);
    }
}

void CodeEditorScreen::handleServerBroadcast(const json& broadcast) {
    string action = broadcast.value("action", "");
    
    if (action == "edit_request") {
        pendingEditRequester = broadcast.value("requester", "Unknown");
        pendingEditFileId = broadcast.value("file_id", -1);
        showEditRequestDialog = true;
        
        string filename = "Unknown File";
        for (size_t i = 0; i < fileIds.size(); i++) {
            if (fileIds[i] == pendingEditFileId) {
                filename = fileList[i];
                break;
            }
        }
        
        editRequestMessage.setString(pendingEditRequester + " wants to edit\n" + filename);
        
        // Center dialog
        float centerX = windowWidth / 2.0f;
        float centerY = windowHeight / 2.0f;
        
        editRequestCard.setPosition({centerX - 200, centerY - 125});
        editRequestTitle.setPosition({centerX - 180, centerY - 110});
        editRequestMessage.setPosition({centerX - 180, centerY - 70});
        
        allowEditBtn.setPosition({centerX - 130, centerY + 50});
        denyEditBtn.setPosition({centerX + 10, centerY + 50});
    }
}

void CodeEditorScreen::requestFileEdit(int fileId) {
    json request;
    request["action"] = "request_file_edit";
    request["file_id"] = fileId;
    request["user_id"] = Session::getInstance().getUserId();
    
    json response = Session::getInstance().getNetworkClient()->sendRequest(request);
    if (response["status"] == "success") {
        if (response.value("granted", false)) {
            currentFileIsLockedByMe = true;
            currentFileIsLocked = true;
            currentFileLockOwner = Session::getInstance().getUsername();
        } else {
            currentFileIsLockedByMe = false;
            currentFileIsLocked = true;
            // The server might send "pending" status or similar? 
            // In Server.cpp, it returns "status": "success", "granted": true/false.
            // If granted is false, it means someone else has it, and request was forwarded?
            // "edit_request_sent" is returned if forwarded.
            
            string status = response.value("status", "");
            if (status == "edit_request_sent") {
                 // Notify user request sent (maybe console or UI)
                 cout << "Edit request sent to owner." << endl;
            }
        }
    }
}

void CodeEditorScreen::releaseFileLock() {
    if (!currentFileIsLockedByMe || currentFileId == -1) return;
    
    json request;
    request["action"] = "release_file_lock";
    request["file_id"] = currentFileId;
    request["user_id"] = Session::getInstance().getUserId();
    
    Session::getInstance().getNetworkClient()->sendRequest(request);
    currentFileIsLockedByMe = false;
    currentFileIsLocked = false;
}

void CodeEditorScreen::fetchFileContent(int fileId) {
    json request;
    request["action"] = "get_file_content";
    request["file_id"] = fileId;
    
    json response = Session::getInstance().getNetworkClient()->sendRequest(request);
    if (response["status"] == "success") {
        code = response.value("content", "");
        currentFileId = fileId;
        cursorPos = code.length();
        updateCodeDisplay();
    }
}

void CodeEditorScreen::saveFile() {
    if (currentFileId == -1) return;
    
    json request;
    request["action"] = "save_file";
    request["file_id"] = currentFileId;
    request["content"] = code;
    
    // Send background save call
    Session::getInstance().getNetworkClient()->sendRequest(request);
    needsSave = false;
}

void CodeEditorScreen::loadMessages() {
    json request;
    request["action"] = "get_messages";
    request["project_id"] = Session::getInstance().getCurrentProjectId();
    
    json response = Session::getInstance().getNetworkClient()->sendRequest(request);
    if (response["status"] == "success") {
        messageDisplay.clear();
        float yPos = 80;
        float chatX = 800.0f - chatWidth + 10;
        float maxTextWidth = chatWidth - 20; // Leave margin
        
        for (auto& msgJson : response["messages"]) {
            string sender = msgJson["sender"];
            string text = msgJson["message"];
            string fullMessage = sender + ": " + text;
            
            // Create text object
            Text msgText(fullMessage, font, 12);
            msgText.setPosition({chatX, yPos});
            msgText.setFillColor(Color(200, 220, 255));
            
            // Check if text needs wrapping
            FloatRect bounds = msgText.getLocalBounds();
            if (bounds.width > maxTextWidth) {
                // Truncate or wrap text
                string displayText = fullMessage;
                while (bounds.width > maxTextWidth && displayText.length() > 3) {
                    displayText = displayText.substr(0, displayText.length() - 1);
                    msgText.setString(displayText + "...");
                    bounds = msgText.getLocalBounds();
                }
            }
            
            messageDisplay.push_back(msgText);
            yPos += 20; // Reduced spacing for better fit
        }
    }
}

void CodeEditorScreen::sendMessage() {
    string text = chatInputField.getString();
    if (text.empty()) return;
    
    json request;
    request["action"] = "send_message";
    request["project_id"] = Session::getInstance().getCurrentProjectId();
    request["sender"] = Session::getInstance().getUsername();
    request["message"] = text;
    
    json response = Session::getInstance().getNetworkClient()->sendRequest(request);
    if (response["status"] == "success") {
        chatInputField.setString("");
        loadMessages();
    }
}

void CodeEditorScreen::createNewFile(const string& name) {
    json request;
    request["action"] = "create_file";
    request["name"] = name;
    request["project_id"] = Session::getInstance().getCurrentProjectId();
    
    json response = Session::getInstance().getNetworkClient()->sendRequest(request);
    if (response["status"] == "success") {
        int newId = response["file_id"];
        loadFiles();
        fetchFileContent(newId);
        // Find index for highlighting
        for (size_t i = 0; i < fileIds.size(); i++) {
            if (fileIds[i] == newId) {
                selectedFileIndex = i;
                break;
            }
        }
    }
}

void CodeEditorScreen::handleFileSelection(Vector2f mousePos) {
    float fileListStartY = isNamingFile ? 100.0f : 70.0f;
    if (mousePos.x < 0 || mousePos.x > sidebarWidth || mousePos.y < fileListStartY || mousePos.y > (windowHeight - 50.0f)) {
        return;
    }
    float lineHeight = 25.0f;
    int clickedIndex = static_cast<int>((mousePos.y - fileListStartY) / lineHeight);
    if (clickedIndex >= 0 && clickedIndex < static_cast<int>(fileList.size())) {
        if (needsSave) saveFile(); // Save previous file
        releaseFileLock(); // Release lock on current file
        
        selectedFileIndex = clickedIndex;
        requestFileEdit(fileIds[selectedFileIndex]); // Request lock for new file
        fetchFileContent(fileIds[selectedFileIndex]);
        
        editorLabel.setString("Code Editor - " + fileList[selectedFileIndex]);
    }
}

void CodeEditorScreen::updateLayout() {
    float topBarHeight = 40.0f;
    float availableHeight = windowHeight - topBarHeight;
    
    // Update sidebar
    sidebarBox.setSize({sidebarWidth, availableHeight});
    sidebarBox.setPosition({0, topBarHeight});
    sidebarHandle.setPosition({sidebarWidth - 2.5f, topBarHeight});
    sidebarHandle.setSize({5, availableHeight});
    
    // Update sidebar title position
    sidebarTitle.setPosition({10, topBarHeight + 5});
    
    // Update file name input box position
    fileNameInputBox.setPosition({10, topBarHeight + 30});
    fileNameInputBox.setSize({sidebarWidth - 20, 25});
    fileNameInputText.setPosition({15, topBarHeight + 33});
    
    // Update new file button
    newFileBtn.setPosition({10, windowHeight - 45.0f});
    newFileBtn.setSize({sidebarWidth - 20, 35});
    
    float editorX = sidebarWidth;
    float editorWidth = windowWidth - sidebarWidth - (chatVisible ? chatWidth : 0);
    
    if (outputVisible) {
        float editorHeight = availableHeight - outputHeight;
        editorBox.setSize({editorWidth, editorHeight});
        editorBox.setPosition({editorX, topBarHeight});
        
        outputBox.setSize({editorWidth, outputHeight});
        outputBox.setPosition({editorX, topBarHeight + editorHeight});
        outputHandle.setPosition({editorX, topBarHeight + editorHeight - 2.5f});
        outputHandle.setSize({editorWidth, 5});
        
        editorLabel.setPosition({editorX + 5, topBarHeight + 5});
        outputLabel.setPosition({editorX + 5, topBarHeight + editorHeight + 5});
        codeText.setPosition({editorX + 10, topBarHeight + 30});
        outputText.setPosition({editorX + 10, topBarHeight + editorHeight + 30});
    } else {
        editorBox.setSize({editorWidth, availableHeight});
        editorBox.setPosition({editorX, topBarHeight});
        outputBox.setSize({editorWidth, 0});
        
        editorLabel.setPosition({editorX + 5, topBarHeight + 5});
        codeText.setPosition({editorX + 10, topBarHeight + 30});
    }
    
    if (chatVisible) {
        chatBox.setSize({chatWidth, availableHeight});
        chatBox.setPosition({windowWidth - chatWidth, topBarHeight});
        chatHandle.setPosition({windowWidth - chatWidth - 2.5f, topBarHeight});
        chatHandle.setSize({5, availableHeight});
        
        // Update chat title with proper text wrapping
        chatTitle.setString("CHAT");
        chatTitle.setPosition({windowWidth - chatWidth + 10, topBarHeight + 5});
        
        // Update chat input field and button
        float chatInputWidth = chatWidth - 80;
        chatInputField.setPosition({windowWidth - chatWidth + 10, windowHeight - 45});
        chatInputField.setSize({chatInputWidth, 35});
        sendChatBtn.setPosition({windowWidth - chatWidth + chatInputWidth + 15, windowHeight - 45});
        sendChatBtn.setSize({60, 35});
        
        // Reload messages with proper wrapping for new width
        loadMessages();
    }
    
    // Update code and output display to recalculate cursor position
    updateCodeDisplay();
    updateOutputDisplay();

    // Position Share Popup (centered)
    sharePopupCard.setPosition({windowWidth / 2 - 150, windowHeight / 2 - 100});
    sharePopupTitle.setPosition({windowWidth / 2 - 140, windowHeight / 2 - 90});
    sharePopupInfo.setPosition({windowWidth / 2 - 140, windowHeight / 2 - 50});
    closeShareBtn.setPosition({windowWidth / 2 - 40, windowHeight / 2 + 50});

    // Update Top Menu Bar
    topMenuBar.setSize({windowWidth, topBarHeight});
    logoutBtn.setPosition({windowWidth - 100, 5});
}

void CodeEditorScreen::toggleOutputPanel() {
    outputVisible = !outputVisible;
    updateLayout();
}

void CodeEditorScreen::drawSidebar(RenderWindow& window) {
    window.draw(sidebarBox);
    window.draw(sidebarTitle);
    
    float fileListStartY = 70;
    if (isNamingFile) {
        window.draw(fileNameInputBox);
        window.draw(fileNameInputText);
        fileListStartY = 100;
    }
    
    float yPos = fileListStartY;
    float lineHeight = 25.0f;
    for (size_t i = 0; i < fileList.size(); i++) {
        if (static_cast<int>(i) == selectedFileIndex) {
            RectangleShape highlight({sidebarWidth - 5.0f, lineHeight - 2});
            highlight.setPosition({2.5f, yPos});
            highlight.setFillColor(Color(60, 80, 120, 150));
            window.draw(highlight);
        }
        bool isLocked = (i < fileLockedStatus.size() && fileLockedStatus[i]);
        string displayText = fileList[i];
        if (isLocked) {
            string owner = (i < fileLockedBy.size()) ? fileLockedBy[i] : "?";
            displayText += " [LOCKED: " + owner + "]";
        }

        Text fileText(displayText, font, 12);
        fileText.setPosition({10, yPos + 3});
        
        if (isLocked) {
             fileText.setFillColor(Color(255, 100, 100));
        } else {
             fileText.setFillColor(Color(200, 220, 240));
        }
        window.draw(fileText);
        yPos += lineHeight;
    }
    newFileBtn.draw(window);
}

void CodeEditorScreen::drawTopMenu(RenderWindow& window) {
    window.draw(topMenuBar);
    fileMenuBtn.draw(window);
    editMenuBtn.draw(window);
    viewMenuBtn.draw(window);
    runMenuBtn.draw(window);
    toggleOutputBtn.draw(window);
    chatBtn.draw(window);
    shareBtn.draw(window);
    backBtn.draw(window);
    logoutBtn.draw(window);
}

AppState CodeEditorScreen::run(RenderWindow& window) {
    loadFiles();
    loadMessages();
    if (!fileIds.empty()) {
        fetchFileContent(fileIds[0]);
    }

    while (window.isOpen()) {
        // Debounce Save Check
        if (needsSave && lastEditClock.getElapsedTime().asSeconds() > 1.0f) {
            saveFile();
        }

        if (showSharePopup) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) return AppState::EXIT;
                
                if (event.type == sf::Event::Resized) {
                    windowWidth = static_cast<float>(event.size.width);
                    windowHeight = static_cast<float>(event.size.height);
                    sf::FloatRect visibleArea(0, 0, windowWidth, windowHeight);
                    window.setView(sf::View(visibleArea));
                    updateLayout();
                }

                if (closeShareBtn.isClicked(event, window)) showSharePopup = false;
            }
            // Update only popup button
            closeShareBtn.update(window);
        } else if (showEditRequestDialog) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) return AppState::EXIT;
                
                if (event.type == sf::Event::Resized) {
                    windowWidth = static_cast<float>(event.size.width);
                    windowHeight = static_cast<float>(event.size.height);
                    sf::FloatRect visibleArea(0, 0, windowWidth, windowHeight);
                    window.setView(sf::View(visibleArea));
                    updateLayout();
                    
                    float centerX = windowWidth / 2.0f;
                    float centerY = windowHeight / 2.0f;
                    editRequestCard.setPosition({centerX - 200, centerY - 125});
                    editRequestTitle.setPosition({centerX - 180, centerY - 110});
                    editRequestMessage.setPosition({centerX - 180, centerY - 70});
                    allowEditBtn.setPosition({centerX - 130, centerY + 50});
                    denyEditBtn.setPosition({centerX + 10, centerY + 50});
                }
                
                if (allowEditBtn.isClicked(event, window)) {
                     json res;
                     res["action"] = "respond_edit_request";
                     res["file_id"] = pendingEditFileId;
                     res["requester"] = pendingEditRequester;
                     res["granted"] = true;
                     Session::getInstance().getNetworkClient()->sendRequest(res);
                     showEditRequestDialog = false;
                }
                if (denyEditBtn.isClicked(event, window)) {
                     json res;
                     res["action"] = "respond_edit_request";
                     res["file_id"] = pendingEditFileId;
                     res["requester"] = pendingEditRequester;
                     res["granted"] = false;
                     Session::getInstance().getNetworkClient()->sendRequest(res);
                     showEditRequestDialog = false;
                }
            }
            allowEditBtn.update(window);
            denyEditBtn.update(window);
        } else {
            pollForUpdates();
            sf::Event event;
            while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                return AppState::EXIT;
            
            if (event.type == sf::Event::Resized) {
                windowWidth = static_cast<float>(event.size.width);
                windowHeight = static_cast<float>(event.size.height);
                sf::FloatRect visibleArea(0, 0, windowWidth, windowHeight);
                window.setView(sf::View(visibleArea));
                updateLayout();
            }
            
            if (chatVisible) {
                chatInputField.handleEvent(event, window);
                if (sendChatBtn.isClicked(event, window)) {
                    sendMessage();
                }
            }

            if (runMenuBtn.isClicked(event, window)) {
                if (!outputVisible) toggleOutputPanel();
                executeCode();
            }
            if (toggleOutputBtn.isClicked(event, window)) toggleOutputPanel();
            if (chatBtn.isClicked(event, window)) {
                chatVisible = !chatVisible;
                updateLayout();
            }
            if (shareBtn.isClicked(event, window)) {
                json req;
                req["action"] = "get_project_info";
                req["project_id"] = Session::getInstance().getCurrentProjectId();
                json res = Session::getInstance().getNetworkClient()->sendRequest(req);
                if (res["status"] == "success") {
                    string info = "Project ID: " + to_string(res["info"]["id"]) + "\nAccess Key: " + to_string(res["info"]["accessKey"]);
                    sharePopupInfo.setString(info);
                    showSharePopup = true;
                }
            }
            if (backBtn.isClicked(event, window)) {
                if (needsSave) saveFile();
                return AppState::PROJECT_SELECT;
            }
            if (newFileBtn.isClicked(event, window)) {
                isNamingFile = true;
                newFileName = "";
                fileNameInputText.setString("");
            }
            if (logoutBtn.isClicked(event, window)) {
                if (needsSave) saveFile();
                Session::getInstance().clearSession();
                return AppState::LOGIN;
            }
            
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                    
                    if (sidebarHandle.getGlobalBounds().contains(mousePos)) {
                        isResizingSidebar = true;
                    } else if (chatVisible && chatHandle.getGlobalBounds().contains(mousePos)) {
                        isResizingChat = true;
                    } else if (outputVisible && outputHandle.getGlobalBounds().contains(mousePos)) {
                        isResizingOutput = true;
                    } else if (mousePos.x <= sidebarWidth) {
                        handleFileSelection(mousePos);
                    } else if (mousePos.x > sidebarWidth && mousePos.x < (chatVisible ? (windowWidth - chatWidth) : windowWidth)) {
                        size_t newPos = getCursorPosFromClick(mousePos);
                        cursorPos = newPos;
                        clearSelection();
                        isSelecting = true;
                        updateCodeDisplay();
                    }
                }
            }

            if (event.type == sf::Event::MouseMoved) {
                Vector2f mousePos = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
                if (isResizingSidebar) {
                    sidebarWidth = max(100.0f, min(400.0f, mousePos.x));
                    updateLayout();
                } else if (isResizingChat) {
                    chatWidth = max(100.0f, min(400.0f, windowWidth - mousePos.x));
                    updateLayout();
                } else if (isResizingOutput) {
                    outputHeight = max(50.0f, min(500.0f, windowHeight - mousePos.y));
                    updateLayout();
                } else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && isSelecting) {
                    size_t newPos = getCursorPosFromClick(mousePos);
                    if (newPos != cursorPos) {
                        selectionEnd = newPos;
                        cursorPos = newPos;
                        updateCodeDisplay();
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Button::Left) {
                    isSelecting = false;
                    isResizingSidebar = false;
                    isResizingChat = false;
                    isResizingOutput = false;
                }
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    char c = static_cast<char>(event.text.unicode);
                    if (isNamingFile) {
                        if (c == '\r' || c == '\n') {
                            if (!newFileName.empty()) {
                                if (newFileName.find(".") == string::npos) newFileName += ".cpp";
                                createNewFile(newFileName);
                            }
                            isNamingFile = false;
                        } else if (c == '\b' && !newFileName.empty()) {
                            newFileName.pop_back();
                            fileNameInputText.setString(newFileName);
                        } else if (c >= 32 && c < 127) {
                            newFileName += c;
                            fileNameInputText.setString(newFileName);
                        }
                    } else if (!chatInputField.getFocused()) {
                        // Regular code input
                        bool changed = false;
                        if (c == '\b') {
                            if (hasSelection()) { deleteSelection(); changed = true; }
                            else if (cursorPos > 0) { code.erase(cursorPos - 1, 1); cursorPos--; changed = true; }
                        } else if (c == '\r' || c == '\n') {
                            if (hasSelection()) deleteSelection();
                            code.insert(cursorPos, "\n"); cursorPos++; clearSelection(); changed = true;
                        } else if (c >= 32) {
                            if (hasSelection()) deleteSelection();
                            code.insert(cursorPos, 1, c); cursorPos++; clearSelection(); changed = true;
                        }
                        if (changed) {
                            updateCodeDisplay();
                            needsSave = true;
                            lastEditClock.restart();
                        }
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                bool shiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
                bool ctrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
                
                if (ctrlPressed) {
                    if (event.key.code == sf::Keyboard::C && hasSelection()) Clipboard::setString(getSelectedText());
                    else if (event.key.code == sf::Keyboard::V) {
                        if (hasSelection()) deleteSelection();
                        string clip = Clipboard::getString();
                        code.insert(cursorPos, clip); cursorPos += clip.length(); clearSelection();
                        updateCodeDisplay(); needsSave = true; lastEditClock.restart();
                    } else if (event.key.code == sf::Keyboard::X && hasSelection()) {
                        Clipboard::setString(getSelectedText()); deleteSelection();
                        updateCodeDisplay(); needsSave = true; lastEditClock.restart();
                    } else if (event.key.code == sf::Keyboard::A) {
                        selectionStart = 0; selectionEnd = code.length(); cursorPos = code.length();
                        updateCodeDisplay();
                    }
                }
                
                if (event.key.code == sf::Keyboard::Left && cursorPos > 0) {
                    if (shiftPressed) { if (!hasSelection()) selectionStart = cursorPos; cursorPos--; selectionEnd = cursorPos; }
                    else { cursorPos--; clearSelection(); }
                    preferredColumn = 0; updateCodeDisplay();
                } else if (event.key.code == sf::Keyboard::Right && cursorPos < code.length()) {
                    if (shiftPressed) { if (!hasSelection()) selectionStart = cursorPos; cursorPos++; selectionEnd = cursorPos; }
                    else { cursorPos++; clearSelection(); }
                    preferredColumn = 0; updateCodeDisplay();
                } else if (event.key.code == sf::Keyboard::Up) {
                    if (shiftPressed) { if (!hasSelection()) selectionStart = cursorPos; moveCursorUp(); selectionEnd = cursorPos; }
                    else { moveCursorUp(); clearSelection(); }
                    updateCodeDisplay();
                } else if (event.key.code == sf::Keyboard::Down) {
                    if (shiftPressed) { if (!hasSelection()) selectionStart = cursorPos; moveCursorDown(); selectionEnd = cursorPos; }
                    else { moveCursorDown(); clearSelection(); }
                    updateCodeDisplay();
                } else if (event.key.code == sf::Keyboard::Home) {
                    if (shiftPressed && !hasSelection()) selectionStart = cursorPos;
                    while (cursorPos > 0 && code[cursorPos - 1] != '\n') cursorPos--;
                    if (shiftPressed) selectionEnd = cursorPos; else clearSelection();
                    preferredColumn = 0; updateCodeDisplay();
                } else if (event.key.code == sf::Keyboard::End) {
                    if (shiftPressed && !hasSelection()) selectionStart = cursorPos;
                    while (cursorPos < code.length() && code[cursorPos] != '\n') cursorPos++;
                    if (shiftPressed) selectionEnd = cursorPos; else clearSelection();
                    preferredColumn = 0; updateCodeDisplay();
                } else if (event.key.code == sf::Keyboard::Tab) {
                    if (hasSelection()) deleteSelection();
                    code.insert(cursorPos, "    "); cursorPos += 4; clearSelection();
                    updateCodeDisplay(); needsSave = true; lastEditClock.restart();
                } else if (event.key.code == sf::Keyboard::Delete) {
                    if (hasSelection()) deleteSelection();
                    else if (cursorPos < code.length()) code.erase(cursorPos, 1);
                    updateCodeDisplay(); needsSave = true; lastEditClock.restart();
                }
            }
        }
    
        // Update
        newFileBtn.update(window);
        runMenuBtn.update(window);
        toggleOutputBtn.update(window);
        chatBtn.update(window);
        shareBtn.update(window);
        backBtn.update(window);
        logoutBtn.update(window);
        if (chatVisible) {
            sendChatBtn.update(window);
        }
    } // This closes the 'else' block which corresponds to (!showSharePopup)
        
    if (showSharePopup) {
        closeShareBtn.update(window);
    }
    particles.update();

        // Draw
        window.clear();
        VertexArray gradient(PrimitiveType::TriangleStrip, 4);
        gradient[0].position = {0, 0}; gradient[0].color = Color(15, 23, 42);
        gradient[1].position = {0, windowHeight}; gradient[1].color = Color(30, 41, 59);
        gradient[2].position = {windowWidth, 0}; gradient[2].color = Color(15, 23, 42);
        gradient[3].position = {windowWidth, windowHeight}; gradient[3].color = Color(51, 65, 85);
        window.draw(gradient);
        particles.draw(window);

        drawTopMenu(window);
        drawSidebar(window);
        window.draw(editorBox);
        if (outputVisible) {
            window.draw(outputBox);
            window.draw(outputLabel);
            window.draw(outputText);
        }
        window.draw(editorLabel);
        drawSelection(window);
        window.draw(codeText);
        if (cursorBlink.getElapsedTime().asSeconds() < 0.5f) window.draw(cursor);
        if (cursorBlink.getElapsedTime().asSeconds() >= 1.0f) cursorBlink.restart();

        if (chatVisible) {
            window.draw(chatBox);
            window.draw(chatTitle);
            for (auto& msg : messageDisplay) window.draw(msg);
            chatInputField.draw(window);
            sendChatBtn.draw(window);
        }

        // Draw Handles Last to ensure they are on top
        if (isResizingSidebar) sidebarHandle.setFillColor(Color(100, 200, 255, 220));
        else sidebarHandle.setFillColor(Color(100, 150, 255, 120));
        window.draw(sidebarHandle);

        if (chatVisible) {
            if (isResizingChat) chatHandle.setFillColor(Color(100, 200, 255, 220));
            else chatHandle.setFillColor(Color(100, 150, 255, 120));
            window.draw(chatHandle);
        }

        if (outputVisible) {
            if (isResizingOutput) outputHandle.setFillColor(Color(100, 200, 255, 220));
            else outputHandle.setFillColor(Color(100, 150, 255, 120));
            window.draw(outputHandle);
        }
        
        if (showSharePopup) {
            window.draw(popupOverlay);
            window.draw(sharePopupCard);
            window.draw(sharePopupTitle);
            window.draw(sharePopupInfo);
            closeShareBtn.draw(window);
        }

        window.display();
    }
    return AppState::EXIT;
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
    
    float lineHeight = font.getLineSpacing(14);
    float relativeY = mousePos.y - codeText.getPosition().y;
    int lineIndex = static_cast<int>(relativeY / lineHeight);
    
    auto lines = splitLines(code);
    if (lineIndex < 0) lineIndex = 0;
    if (lineIndex >= static_cast<int>(lines.size())) lineIndex = lines.size() - 1;
    
    size_t currentPos = 0;
    for (int i = 0; i < lineIndex; ++i) {
        currentPos += lines[i].length() + 1;
    }
    
    string& line = lines[lineIndex];
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
            currentLine = i;
            columnPos = cursorPos - pos;
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
            currentLine = i;
            columnPos = cursorPos - pos;
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
