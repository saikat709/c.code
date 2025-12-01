#include "NetworkClient.hpp"

using namespace std;

NetworkClient::NetworkClient() : clientSocket(INVALID_SOCKET), isConnected(false) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
    }
}

NetworkClient::~NetworkClient() {
    if (clientSocket != INVALID_SOCKET) closesocket(clientSocket);
    WSACleanup();
}

bool NetworkClient::connectToServer(const string& ip, int port) {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed" << endl;
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    serverAddr.sin_port = htons(port);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Connection failed" << endl;
        closesocket(clientSocket);
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

    string requestStr = request.dump();
    if (send(clientSocket, requestStr.c_str(), requestStr.length(), 0) == SOCKET_ERROR) {
        cerr << "Send failed" << endl;
        return {{"status", "error"}, {"message", "Send failed"}};
    }

    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
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
