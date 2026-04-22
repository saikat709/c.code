#include <iostream>
#include <chrono>
#include "NetworkClient.hpp"
#include "json.hpp"
#include "huffman.hpp"

using namespace std;

NetworkClient::NetworkClient() : clientSocket(-1), isConnected(false), stopListener(false) {
    printf("NetworkClient initialized\n");
}

NetworkClient::~NetworkClient() {
    stopListener = true;
    if (clientSocket != -1) shutdown(clientSocket, SHUT_RDWR);
    if (listenerThread.joinable()) listenerThread.join();
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
    stopListener = false;
    if (!listenerThread.joinable()) {
        listenerThread = thread(&NetworkClient::listenForBroadcasts, this);
    }

    cout << "Connected to server at " << ip << ":" << port << endl;
    return true;
}

void NetworkClient::listenForBroadcasts() {
    Huffman huffman;
    char buffer[4096];

    while (!stopListener && isConnected) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            if (!stopListener) {
                isConnected = false;
                responseCv.notify_all();
            }
            break;
        }

        buffer[bytesReceived] = '\0';
        try {
            string responseStr(buffer);
            string decompressedResponse = huffman.decompress(responseStr);
            json payload = json::parse(decompressedResponse);

            if (payload.contains("status")) {
                {
                    lock_guard<mutex> lock(responseMutex);
                    pendingResponses.push(payload);
                }
                responseCv.notify_one();
            } else {
                lock_guard<mutex> lock(pendingMutex);
                pendingNotifications.push_back(payload);
            }
        } catch (const json::parse_error& e) {
            cerr << "JSON Parse Error in listener: " << e.what() << endl;
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

    lock_guard<mutex> sendLock(sendMutex);

    Huffman huffman;
    string requestStr = huffman.compress(request.dump());
    if (send(clientSocket, requestStr.c_str(), requestStr.size(), 0) == -1) {
        cerr << "Send failed" << endl;
        return {
            {"status", "error"},
            {"message", "Send failed"}
        };
    }

    unique_lock<mutex> lock(responseMutex);
    responseCv.wait(lock, [this]() {
        return !pendingResponses.empty() || !isConnected || stopListener;
    });

    if (pendingResponses.empty()) {
        return {
            {"status", "error"},
            {"message", "Receive failed"}
        };
    }

    json response = pendingResponses.front();
    pendingResponses.pop();
    return response;
}

vector<json> NetworkClient::getPendingNotifications() {
    lock_guard<mutex> lock(pendingMutex);
    vector<json> temp = pendingNotifications;
    pendingNotifications.clear();
    return temp;
}
