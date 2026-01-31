#pragma once
#include <string>
#include "NetworkClient.hpp"

class Session {
public:
    static Session& getInstance() {
        static Session instance;
        return instance;
    }

    int getUserId() const { return userId; }
    void setUserId(int id) { userId = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }

    int getCurrentProjectId() const { return currentProjectId; }
    void setCurrentProjectId(int id) { currentProjectId = id; }

    NetworkClient* getNetworkClient() { return networkClient; }
    void setNetworkClient(NetworkClient* client) { networkClient = client; }

    void saveSession(int lastState);
    bool loadSession(int& lastState);
    void clearSession();

private:
    Session() : userId(0), currentProjectId(0), networkClient(nullptr) {}
    int userId;
    std::string username;
    int currentProjectId;
    NetworkClient* networkClient;
};
