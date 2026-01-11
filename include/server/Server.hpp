#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <thread>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

class Server {
    int serverSocket;
    struct sockaddr_in serverAddr;
    bool isRunning;

public:
    Server();
    ~Server();
    bool start(int port);
    void listenForConnections();
    void handleClient(int clientSocket);
};
