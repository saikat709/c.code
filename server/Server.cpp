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

void Server::sendToClient(int socketFd, const json& response) {
    Huffman huffman;
    string responseStr = response.dump();
    string compressedResponse = huffman.compress(responseStr);
    send(socketFd, compressedResponse.c_str(), compressedResponse.length(), 0);
}

void Server::broadcastToProject(int projectId, const json& message, int excludeSocket) {
    lock_guard<mutex> lock(clientsMutex);
    for (const auto& [socketFd, clientInfo] : connectedClients) {
        if (clientInfo.projectId == projectId && socketFd != excludeSocket) {
            sendToClient(socketFd, message);
        }
    }
}

void Server::handleClient(int clientSocket) {
    char buffer[4096];
    int currentUserId = 0;
    int currentProjectId = 0;
    string currentUsername;

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Client disconnected" << endl;
            
            // Clean up: remove from connected clients and unlock any files
            {
                lock_guard<mutex> lock(clientsMutex);
                connectedClients.erase(clientSocket);
            }
            
            // Release any file locks held by this client
            {
                lock_guard<mutex> lock(fileLocksMutex);
                for (auto it = fileLocks.begin(); it != fileLocks.end(); ) {
                    if (it->second.socketFd == clientSocket) {
                        DBActions::unlockFile(*db, it->first, it->second.userId);
                        it = fileLocks.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
            
            close(clientSocket);
            break;
        }

        buffer[bytesReceived] = '\0';
        string receivedData(buffer);
        cout << "Length: " << receivedData.size() << endl;
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
                    currentUserId = user.getId();
                    currentUsername = user.getUsername();
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
            } else if (action == "set_project") {
                // Track which project this client is in
                currentProjectId = request["project_id"];
                {
                    lock_guard<mutex> lock(clientsMutex);
                    connectedClients[clientSocket] = {currentUserId, currentProjectId, currentUsername};
                }
                response["status"] = "success";
            } else if (action == "get_files") {
                int projectId = request["project_id"];
                json files = DBActions::getFiles(*db, projectId);
                
                // Add lock information to each file
                for (auto& file : files) {
                    int fileId = file["id"];
                    if (DBActions::isFileLocked(*db, fileId)) {
                        json lockInfo = DBActions::getFileLockInfo(*db, fileId);
                        file["locked"] = true;
                        file["lockedBy"] = lockInfo["username"];
                        file["lockedByUserId"] = lockInfo["userId"];
                    } else {
                        file["locked"] = false;
                    }
                }
                
                response["status"] = "success";
                response["files"] = files;
            } else if (action == "create_file") {
                string name = request["name"];
                int projectId = request["project_id"];
                int fileId = DBActions::createFile(*db, name, projectId);
                if (fileId != -1) {
                    response["status"] = "success";
                    response["file_id"] = fileId;
                    
                    // Broadcast new file to all project members
                    json broadcast;
                    broadcast["type"] = "file_created";
                    broadcast["file_id"] = fileId;
                    broadcast["file_name"] = name;
                    broadcast["creator"] = currentUsername;
                    broadcastToProject(projectId, broadcast, clientSocket);
                } else {
                    response["status"] = "error";
                }
            } else if (action == "request_file_edit") {
                int fileId = request["file_id"];
                int userId = request["user_id"];
                
                if (DBActions::isFileLocked(*db, fileId)) {
                    json lockInfo = DBActions::getFileLockInfo(*db, fileId);
                    int lockOwnerId = lockInfo["userId"];
                    
                    if (lockOwnerId == userId) {
                        // User already has the lock
                        response["status"] = "success";
                        response["granted"] = true;
                    } else {
                        // File is locked by someone else - send request
                        response["status"] = "pending";
                        response["message"] = "Edit request sent to " + lockInfo["username"].get<string>();
                        
                        // Find the lock owner's socket and send them a request
                        lock_guard<mutex> lock(clientsMutex);
                        for (const auto& [socketFd, clientInfo] : connectedClients) {
                            if (clientInfo.userId == lockOwnerId) {
                                json editRequest;
                                editRequest["type"] = "edit_request";
                                editRequest["file_id"] = fileId;
                                editRequest["requester"] = currentUsername;
                                editRequest["requester_id"] = userId;
                                editRequest["requester_socket"] = clientSocket;
                                sendToClient(socketFd, editRequest);
                                break;
                            }
                        }
                    }
                } else {
                    // File is not locked, grant access
                    if (DBActions::lockFile(*db, fileId, userId)) {
                        lock_guard<mutex> lock(fileLocksMutex);
                        fileLocks[fileId] = {userId, currentUsername, clientSocket};
                        
                        response["status"] = "success";
                        response["granted"] = true;
                        
                        // Broadcast lock status to project
                        json broadcast;
                        broadcast["type"] = "file_locked";
                        broadcast["file_id"] = fileId;
                        broadcast["locked_by"] = currentUsername;
                        broadcastToProject(currentProjectId, broadcast, clientSocket);
                    } else {
                        response["status"] = "error";
                        response["message"] = "Failed to lock file";
                    }
                }
            } else if (action == "respond_edit_request") {
                bool allowed = request["allowed"];
                int fileId = request["file_id"];
                int requesterId = request["requester_id"];
                int requesterSocket = request["requester_socket"];
                
                json responseToRequester;
                responseToRequester["type"] = "edit_request_response";
                responseToRequester["file_id"] = fileId;
                responseToRequester["allowed"] = allowed;
                
                if (allowed) {
                    // Transfer the lock
                    DBActions::unlockFile(*db, fileId, currentUserId);
                    DBActions::lockFile(*db, fileId, requesterId);
                    
                    {
                        lock_guard<mutex> lock(fileLocksMutex);
                        // Update lock info
                        lock_guard<mutex> clientLock(clientsMutex);
                        if (connectedClients.count(requesterSocket)) {
                            fileLocks[fileId] = {requesterId, connectedClients[requesterSocket].username, requesterSocket};
                        }
                    }
                    
                    responseToRequester["message"] = "Edit access granted";
                    
                    // Broadcast lock change
                    json broadcast;
                    broadcast["type"] = "file_locked";
                    broadcast["file_id"] = fileId;
                    {
                        lock_guard<mutex> lock(clientsMutex);
                        if (connectedClients.count(requesterSocket)) {
                            broadcast["locked_by"] = connectedClients[requesterSocket].username;
                        }
                    }
                    broadcastToProject(currentProjectId, broadcast);
                } else {
                    responseToRequester["message"] = "Edit request denied";
                }
                
                sendToClient(requesterSocket, responseToRequester);
                response["status"] = "success";
            } else if (action == "release_file_lock") {
                int fileId = request["file_id"];
                int userId = request["user_id"];
                
                if (DBActions::unlockFile(*db, fileId, userId)) {
                    lock_guard<mutex> lock(fileLocksMutex);
                    fileLocks.erase(fileId);
                    
                    response["status"] = "success";
                    
                    // Broadcast unlock
                    json broadcast;
                    broadcast["type"] = "file_unlocked";
                    broadcast["file_id"] = fileId;
                    broadcastToProject(currentProjectId, broadcast, clientSocket);
                } else {
                    response["status"] = "error";
                }
            } else if (action == "get_file_content") {
                int fileId = request["file_id"];
                response["status"] = "success";
                response["content"] = DBActions::getFileContent(*db, fileId);
                
                // Include lock status
                if (DBActions::isFileLocked(*db, fileId)) {
                    json lockInfo = DBActions::getFileLockInfo(*db, fileId);
                    response["locked"] = true;
                    response["lockedBy"] = lockInfo["username"];
                    response["lockedByUserId"] = lockInfo["userId"];
                } else {
                    response["locked"] = false;
                }
            } else if (action == "save_file") {
                int fileId = request["file_id"];
                string content = request["content"];
                int userId = request["user_id"];
                
                // Verify user has the lock
                int lockOwner = DBActions::getFileLockOwner(*db, fileId);
                if (lockOwner == userId || lockOwner == -1) {
                    if (DBActions::updateFileContent(*db, fileId, content)) {
                        response["status"] = "success";
                        
                        // Broadcast file update to project members
                        json broadcast;
                        broadcast["type"] = "file_updated";
                        broadcast["file_id"] = fileId;
                        broadcast["updated_by"] = currentUsername;
                        broadcastToProject(currentProjectId, broadcast, clientSocket);
                    } else {
                        response["status"] = "error";
                    }
                } else {
                    response["status"] = "error";
                    response["message"] = "You don't have edit permission for this file";
                }
            } else if (action == "send_message") {
                int projectId = request["project_id"];
                string sender = request["sender"];
                string message = request["message"];
                if (DBActions::sendMessage(*db, projectId, sender, message)) {
                    response["status"] = "success";
                    
                    // Get the new message ID
                    json messages = DBActions::getMessages(*db, projectId);
                    int newMessageId = messages.empty() ? 0 : messages.back()["id"].get<int>();

                    
                    // Broadcast new message to all project members
                    json broadcast;
                    broadcast["type"] = "new_message";
                    broadcast["id"] = newMessageId;
                    broadcast["sender"] = sender;
                    broadcast["message"] = message;
                    broadcastToProject(projectId, broadcast, clientSocket);
                } else {
                    response["status"] = "error";
                }
            } else if (action == "get_messages") {
                int projectId = request["project_id"];
                response["status"] = "success";
                response["messages"] = DBActions::getMessages(*db, projectId);
            } else if (action == "get_messages_since") {
                int projectId = request["project_id"];
                int lastMessageId = request.value("last_message_id", 0);
                response["status"] = "success";
                response["messages"] = DBActions::getMessagesSince(*db, projectId, lastMessageId);
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
