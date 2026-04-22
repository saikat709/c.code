#include "CodeEditorScreen.hpp"
#include "Session.hpp"
#include "json.hpp"

using namespace std;
using namespace sf;

void CodeEditorScreen::loadFiles() {
    json request;
    request["action"] = "get_files";
    request["project_id"] = Session::getInstance().getCurrentProjectId();
    
    json response = Session::getInstance().getNetworkClient()->sendRequest(request);
    if (response["status"] == "success") {
        fileList.clear();
        fileIds.clear();
        
        for (auto& fileJson : response["files"]) {
            fileList.push_back(fileJson["name"]);
            fileIds.push_back(fileJson["id"]);
        }
        
        if (currentFileId == -1 && !fileIds.empty()) {
            selectedFileIndex = 0;
            fetchFileContent(fileIds[0]);
        }
    }
}

void CodeEditorScreen::pollForUpdates() {
    auto notifications = Session::getInstance().getNetworkClient()->getPendingNotifications();
    for (const auto& notif : notifications) {
         handleServerBroadcast(notif);
    }
}

void CodeEditorScreen::handleServerBroadcast(const json& broadcast) {
    int currentProjectId = Session::getInstance().getCurrentProjectId();
    int incomingProjectId = broadcast.value("project_id", 0);
    if (incomingProjectId != 0 && incomingProjectId != currentProjectId) {
        return;
    }

    string kind = broadcast.value("type", broadcast.value("action", ""));

    if (kind == "new_message") {
        int messageId = broadcast.value("id", 0);
        if (messageId > lastMessageId) lastMessageId = messageId;
        string sender = broadcast.value("sender", "");
        string text = broadcast.value("message", "");
        if (!sender.empty()) {
            string fullMessage = sender + ": " + text;
            Text msgText(fullMessage, font, 12);
            float chatX = 800.0f - chatWidth + 10;
            float yPos = messageDisplay.empty() ? 80.0f : (messageDisplay.back().getPosition().y + 20.0f);
            msgText.setPosition({chatX, yPos});
            msgText.setFillColor(Color(200, 220, 255));
            messageDisplay.push_back(msgText);
        }
    } else if (kind == "file_created") {
        int newFileId = broadcast.value("file_id", -1);
        string fileName = broadcast.value("file_name", "");
        if (newFileId != -1 && !fileName.empty()) {
            fileList.push_back(fileName);
            fileIds.push_back(newFileId);
        }
    } else if (kind == "file_updated") {
        int updatedFileId = broadcast.value("file_id", -1);
        if (updatedFileId != -1 && updatedFileId == currentFileId) {
            fetchFileContent(updatedFileId);
        }
    }
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
    request["user_id"] = Session::getInstance().getUserId();
    request["project_id"] = Session::getInstance().getCurrentProjectId();
    request["content"] = code;
    
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
        float maxTextWidth = chatWidth - 20;
        
        for (auto& msgJson : response["messages"]) {
            string sender = msgJson["sender"];
            string text = msgJson["message"];
            string fullMessage = sender + ": " + text;
            
            Text msgText(fullMessage, font, 12);
            msgText.setPosition({chatX, yPos});
            msgText.setFillColor(Color(200, 220, 255));
            
            FloatRect bounds = msgText.getLocalBounds();
            if (bounds.width > maxTextWidth) {
                string displayText = fullMessage;
                while (bounds.width > maxTextWidth && displayText.length() > 3) {
                    displayText = displayText.substr(0, displayText.length() - 1);
                    msgText.setString(displayText + "...");
                    bounds = msgText.getLocalBounds();
                }
            }
            
            messageDisplay.push_back(msgText);
            yPos += 20;
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
        int messageId = response.value("id", 0);
        if (messageId > lastMessageId) lastMessageId = messageId;
        string sender = response.value("sender", Session::getInstance().getUsername());
        string msgBody = response.value("message", text);
        string fullMessage = sender + ": " + msgBody;
        Text msgText(fullMessage, font, 12);
        float chatX = 800.0f - chatWidth + 10;
        float yPos = messageDisplay.empty() ? 80.0f : (messageDisplay.back().getPosition().y + 20.0f);
        msgText.setPosition({chatX, yPos});
        msgText.setFillColor(Color(200, 220, 255));
        messageDisplay.push_back(msgText);
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
        for (size_t i = 0; i < fileIds.size(); i++) {
            if (fileIds[i] == newId) {
                selectedFileIndex = i;
                break;
            }
        }
        if (selectedFileIndex >= 0 && selectedFileIndex < static_cast<int>(fileList.size())) {
            editorLabel.setString("Code Editor - " + fileList[selectedFileIndex]);
        }
        updateLayout();
    }
}
