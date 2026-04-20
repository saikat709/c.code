#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <map>
#include <mutex>
#include <vector>
#include <string>
#include "json.hpp"


class Server {
    int serverSocket;
    struct sockaddr_in serverAddr;
    bool isRunning;
    class Database* db;

    // Track connected clients: socket -> {userId, projectId, username}
    struct ClientInfo {
        int userId = 0;
        int projectId = 0;
        std::string username;
    };
    std::map<int, ClientInfo> connectedClients;
    std::mutex clientsMutex;

    // File lock tracking: fileId -> {lockedByUserId, lockedByUsername}
    struct FileLock {
        int userId;
        std::string username;
        int socketFd;
    };
    std::map<int, FileLock> fileLocks;
    std::mutex fileLocksMutex;

    // Pending edit requests: fileId -> list of {requestingUserId, requestingSocket}
    struct EditRequest {
        int requestingUserId;
        std::string requestingUsername;
        int requestingSocket;
    };
    std::map<int, std::vector<EditRequest>> pendingEditRequests;
    std::mutex editRequestsMutex;

    void broadcastToProject(int projectId, const json& message, int excludeSocket = -1);
    void sendToClient(int socketFd, const json& response);

public:
    Server(class Database* db);
    ~Server();
    bool start(int port);
    void listenForConnections();
    void handleClient(int clientSocket);
};