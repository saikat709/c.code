#include "Server.hpp"
#include "json.hpp"
#include <iostream>

using namespace std;
using json = nlohmann::json;

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
    char buffer[4096];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Client disconnected" << endl;
            closesocket(clientSocket);
            break;
        }
        
        buffer[bytesReceived] = '\0';
        string receivedData(buffer);
        cout << "Received: " << receivedData << endl;
        
        try {
            json request = json::parse(receivedData);
            string action = request["action"];
            json response;

            if (action == "login") {
                string username = request["username"];
                string password = request["password"];
                cout << "Action: Login, Username: " << username << endl;
                
                // TODO: Validate against database
                if (username == "admin" && password == "admin") {
                    response["status"] = "success";
                    response["message"] = "Login successful";
                } else {
                    response["status"] = "error";
                    response["message"] = "Invalid credentials";
                }
            } else if (action == "register") {
                string username = request["username"];
                string password = request["password"];
                string email = request["email"];
                cout << "Action: Register, Username: " << username << ", Email: " << email << endl;

                // TODO: Save to database
                response["status"] = "success";
                response["message"] = "Registration successful";
            } else {
                response["status"] = "error";
                response["message"] = "Unknown action";
            }

            string responseStr = response.dump();
            send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
            cout << "Sent: " << responseStr << endl;

        } catch (const json::parse_error& e) {
            cerr << "JSON Parse Error: " << e.what() << endl;
            string errorMsg = "{\"status\":\"error\",\"message\":\"Invalid JSON\"}";
            send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }
}
