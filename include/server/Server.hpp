#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

class Server {
    SOCKET serverSocket;
    struct sockaddr_in serverAddr;
    bool isRunning;

public:
    Server();
    ~Server();
    bool start(int port);
    void listenForConnections();
    void handleClient(SOCKET clientSocket);
};
