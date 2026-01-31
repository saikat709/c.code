#include "Server.hpp"
#include "json.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include "huffman.hpp"
#include "actions.hpp"
using namespace std;

Server::Server(Database* database) : serverSocket(-1), isRunning(false), db(database) { }

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
    char buffer[4096];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Client disconnected" << endl;
            close(clientSocket);
            break;
        }

        buffer[bytesReceived] = '\0';
        string receivedData(buffer);
        cout << "Length: " << receivedData.size() << endl;
        // cout << "Received: " << receivedData << endl;
        Huffman huffman;

        try {
            string decompressedData = huffman.decompress(receivedData);
            json request = json::parse(decompressedData);
            string action = request["action"];
            json response;

            if (action == "login") {
                string username = request["username"];
                string password = request["password"];
                if (DBActions::loginUser(*db, username, password)) {
                    User user = DBActions::getUserByUsername(*db, username);
                    response["status"] = "success";
                    response["user_id"] = user.getId();
                    response["username"] = user.getUsername();
                } else {
                    response["status"] = "error";
                    response["message"] = "Invalid credentials";
                }
            } else if (action == "register") {
                string username = request["username"];
                string password = request["password"];
                if (DBActions::createUser(*db, username, password)) {
                    response["status"] = "success";
                } else {
                    response["status"] = "error";
                    response["message"] = "Registration failed (user might exist)";
                }
            } else if (action == "get_projects") {
                int userId = request["user_id"];
                response["status"] = "success";
                response["projects"] = DBActions::getProjects(*db, userId);
            } else if (action == "create_project") {
                string name = request["name"];
                int userId = request["user_id"];
                if (DBActions::createProject(*db, name, userId)) {
                    response["status"] = "success";
                } else {
                    response["status"] = "error";
                }
            } else if (action == "get_files") {
                int projectId = request["project_id"];
                response["status"] = "success";
                response["files"] = DBActions::getFiles(*db, projectId);
            } else if (action == "create_file") {
                string name = request["name"];
                int projectId = request["project_id"];
                int fileId = DBActions::createFile(*db, name, projectId);
                if (fileId != -1) {
                    response["status"] = "success";
                    response["file_id"] = fileId;
                } else {
                    response["status"] = "error";
                }
            } else if (action == "get_file_content") {
                int fileId = request["file_id"];
                response["status"] = "success";
                response["content"] = DBActions::getFileContent(*db, fileId);
            } else if (action == "save_file") {
                int fileId = request["file_id"];
                string content = request["content"];
                if (DBActions::updateFileContent(*db, fileId, content)) {
                    response["status"] = "success";
                } else {
                    response["status"] = "error";
                }
            } else if (action == "send_message") {
                int projectId = request["project_id"];
                string sender = request["sender"];
                string message = request["message"];
                if (DBActions::sendMessage(*db, projectId, sender, message)) {
                    response["status"] = "success";
                } else {
                    response["status"] = "error";
                }
            } else if (action == "get_messages") {
                int projectId = request["project_id"];
                response["status"] = "success";
                response["messages"] = DBActions::getMessages(*db, projectId);
            } else if (action == "join_project") {
                int userId = request["user_id"];
                int projectId = request["project_id"];
                int accessKey = request["access_key"];
                if (DBActions::joinProject(*db, userId, projectId, accessKey)) {
                    response["status"] = "success";
                } else {
                    response["status"] = "error";
                    response["message"] = "Invalid ID or Access Key";
                }
            } else if (action == "get_project_info") {
                int projectId = request["project_id"];
                response["status"] = "success";
                response["info"] = DBActions::getProjectAccessInfo(*db, projectId);
            } else {
                response["status"] = "error";
                response["message"] = "Unknown action " + action;
            }

            string responseStr = response.dump();
            string compressedResponse = huffman.compress(responseStr);
            send(clientSocket, compressedResponse.c_str(), compressedResponse.length(), 0);
        } catch (const json::parse_error& e) {
            cerr << "JSON Parse Error: " << e.what() << endl;
            string errorMsg = "{\"status\":\"error\",\"message\":\"Invalid JSON\"}";
            string compressedErrorMsg = huffman.compress(errorMsg);
            send(clientSocket, compressedErrorMsg.c_str(), compressedErrorMsg.length(), 0);
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }
}
