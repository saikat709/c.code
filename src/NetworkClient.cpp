#include "NetworkClient.hpp"
#include <iostream>
#include "json.hpp"
#include "huffman.hpp"

using namespace std;

NetworkClient::NetworkClient() : clientSocket(-1), isConnected(false) {
    printf("NetworkClient initialized\n");
}

NetworkClient::~NetworkClient() {
    if (clientSocket != -1) close(clientSocket); // use close() on Linux
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
    return true;
}

json NetworkClient::sendRequest(const json& request) {
    if (!isConnected) {
        cerr << "Not connected to server" << endl;
        return {{"status", "error"}, {"message", "Not connected"}};
    }

    string requestData = request.dump();
    cout << "Sending request: " << requestData << endl;
    string requestStr = Huffman::compress(requestData);
    cout << "Compressed request: " << requestStr << endl;
    if (send(clientSocket, requestStr.c_str(), requestStr.size(), 0) == -1) {
        cerr << "Send failed" << endl;
        return {{"status", "error"}, {"message", "Send failed"}};
    }

    char buffer[16096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        try {
            return json::parse(buffer);
        } catch (const json::parse_error& e) {
            cerr << "JSON Parse Error: " << e.what() << endl;
            return {{"status", "error"}, {"message", "Invalid JSON response"}};
        }
    } else {
        cerr << "Receive failed or connection closed" << endl;
        return {{"status", "error"}, {"message", "Receive failed"}};
    }
}
