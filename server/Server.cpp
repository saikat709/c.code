#include "Server.hpp"
#include "json.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include "huffman.hpp"
using namespace std;

Server::Server() : serverSocket(-1), isRunning(false) { }

Server::~Server() {
    if (serverSocket != -1) close(serverSocket);
}

bool Server::start(int port) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Socket creation failed" << endl;
        return false;
    }

    // Allow quick restart if the port is in TIME_WAIT
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        cerr << "setsockopt(SO_REUSEADDR) failed: " << strerror(errno) << endl;
        close(serverSocket);
        serverSocket = -1;
        return false;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Bind failed: " << strerror(errno) << endl;
        close(serverSocket);
        serverSocket = -1;
        return false;
    }

    if (listen(serverSocket, SOMAXCONN) == -1) {
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
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            if (isRunning) cerr << "Accept failed" << endl;
            continue;
        }

        cout << "New client connected!" << endl;
        thread(&Server::handleClient, this, clientSocket).detach();
    }
}

void Server::handleClient(int clientSocket) {
    char buffer[16096];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Client disconnected" << endl;
            close(clientSocket);
            break;
        }
        
        buffer[bytesReceived] = '\0';
        string receivedData(buffer);
        cout << "Received: " << receivedData << endl;
        
        try {
            cout << "Received data: " << receivedData << endl;
            string decompressedData = Huffman::decompress(receivedData);
            cout << "Decompressed data: " << decompressedData << endl;
            json request = json::parse(decompressedData);
            cout << "Parsed JSON request: " << request.dump() << endl;
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
