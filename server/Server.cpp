#include "Server.hpp"

using namespace std;

Server::Server() : serverSocket(INVALID_SOCKET), isRunning(false) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
    }
}

Server::~Server() {
    if (serverSocket != INVALID_SOCKET) closesocket(serverSocket);
    WSACleanup();
}

bool Server::start(int port) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed" << endl;
        return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed with error: " << WSAGetLastError() << endl;
        return false;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed" << endl;
        return false;
    }

    isRunning = true;
    cout << "Server listening on 127.0.0.1:" << port << endl;
    
    // Start listening thread
    thread(&Server::listenForConnections, this).detach();
    
    return true;
}

void Server::listenForConnections() {
    while (isRunning) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            if (isRunning) cerr << "Accept failed" << endl;
            continue;
        }

        cout << "New client connected!" << endl;
        thread(&Server::handleClient, this, clientSocket).detach();
    }
}

void Server::handleClient(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Client disconnected" << endl;
            closesocket(clientSocket);
            break;
        }
        
        buffer[bytesReceived] = '\0';
        cout << "Received: " << buffer << endl;
        
        // Echo back for now
        send(clientSocket, buffer, bytesReceived, 0);
    }
}
