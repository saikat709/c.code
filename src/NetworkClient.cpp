#include <iostream>
#include "NetworkClient.hpp"
#include "json.hpp"
#include "huffman.hpp"
#include <thread>
#include <atomic>

using namespace std;

NetworkClient::NetworkClient() : clientSocket(-1), isConnected(false) {
    printf("NetworkClient initialized\n");
    listenerThread = nullptr;
}

NetworkClient::~NetworkClient() {
    stopListener = true;
    if (listenerThread && listenerThread->joinable()) listenerThread->join();
    delete listenerThread;
    if (clientSocket != -1) close(clientSocket);
}

bool NetworkClient::connectToServer(const string& ip, int port) {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Socket creation failed" << endl;
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        cerr << "Invalid IP address" << endl;
        close(clientSocket);
        return false;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Connection failed" << endl;
        close(clientSocket);
        return false;
    }

    isConnected = true;
    cout << "Connected to server at " << ip << ":" << port << endl;
    stopListener = false;
    if (!listenerThread) listenerThread = new std::thread(&NetworkClient::listenForBroadcasts, this);
    return true;

}

void NetworkClient::listenForBroadcasts() {
    Huffman huffman;
    char buffer[4096];
    while (!stopListener && isConnected) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), MSG_DONTWAIT);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            try {
                string responseStr(buffer);
                string decompressedResponse = huffman.decompress(responseStr);
                json response = json::parse(decompressedResponse);
                // If it's a broadcast (has 'type' or 'action' and not a direct response)
                if ((response.contains("type") || response.contains("action")) && !response.contains("status")) {
                    pendingNotifications.push_back(response);
                }
            } catch (...) {}
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
}

json NetworkClient::sendRequest(const json& request) {
    if (!isConnected) {
        cerr << "Not connected to server" << endl;
        return {
            {"status", "error"}, 
            {"message", "Not connected"}
        };
    }

    string requestData = request.dump();
    // cout << "Sending request: " << requestData << endl;
    Huffman huffman;
    string requestStr = huffman.compress(requestData);
    // cout << "Compressed request: " << requestStr << endl;
    if (send(clientSocket, requestStr.c_str(), requestStr.size(), 0) == -1) {
        cerr << "Send failed" << endl;
        return {
            {"status", "error"}, 
            {"message", "Send failed"}
        };
    }

    char buffer[4096];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            try {
                string responseStr(buffer);
                string decompressedResponse = huffman.decompress(responseStr);
                json response = json::parse(decompressedResponse);
                
                // If it's an asynchronous notification/event
                if (response.contains("action") && response["action"] == "edit_request") {
                    pendingNotifications.push_back(response);
                    // cout << "Queued async notification: " << response["action"] << endl;
                    continue; // Wait for the expected synchronous response
                }
                
                return response;
            } catch (const json::parse_error& e) {
                cerr << "JSON Parse Error: " << e.what() << endl;
                return {
                    {"status", "error"}, 
                    {"message", "Invalid JSON response"}
                };
            }
        } else {
            cerr << "Receive failed or connection closed" << endl;
            return {
                {"status", "error"}, 
                {"message", "Receive failed"}
            };
        }
    }
}

vector<json> NetworkClient::getPendingNotifications() {
    vector<json> temp = pendingNotifications;
    pendingNotifications.clear();
    return temp;
}
