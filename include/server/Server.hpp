#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>


class Server {
    int serverSocket;
    struct sockaddr_in serverAddr;
    bool isRunning;
    class Database* db;

public:
    Server(class Database* db);
    ~Server();
    bool start(int port);
    void listenForConnections();
    void handleClient(int clientSocket);
};