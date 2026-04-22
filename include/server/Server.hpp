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

using namespace std;


class Server {
    int serverSocket;
    struct sockaddr_in serverAddr;
    bool isRunning;
    class Database* db;

    struct ClientInfo {
        int userId = 0;
        int projectId = 0;
        string username;
    };
    map<int, ClientInfo> connectedClients;
    mutex clientsMutex;

    void broadcastToAll(const json& message, int excludeSocket = -1);
    void broadcastToProject(int projectId, const json& message, int excludeSocket = -1);
    void sendToClient(int socketFd, const json& response);

public:
    Server(class Database* db);
    ~Server();
    bool start(int port);
    void listenForConnections();
    void handleClient(int clientSocket);
};