#include "Session.hpp"
#include <fstream>
#include <json.hpp>
#include <iostream>

using namespace std;

void Session::saveSession(int lastState) {
    json j;
    j["userId"] = userId;
    j["username"] = username;
    j["currentProjectId"] = currentProjectId;
    j["lastState"] = lastState;
    
    ofstream file("session.json");
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    }
}

bool Session::loadSession(int& lastState) {
    ifstream file("session.json");
    if (!file.is_open()) return false;
    
    try {
        json j;
        file >> j;
        userId = j.value("userId", 0);
        username = j.value("username", "");
        currentProjectId = j.value("currentProjectId", 0);
        lastState = j.value("lastState", 0);
        file.close();
        return userId != 0;
    } catch (...) {
        return false;
    }
}

void Session::clearSession() {
    userId = 0;
    username = "";
    currentProjectId = 0;
    remove("session.json");
}
