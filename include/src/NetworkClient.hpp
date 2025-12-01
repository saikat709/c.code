#pragma once

#define NOMINMAX
#include <winsock2.h>
#include <string>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    bool connectToServer(const std::string& ip, int port);
    json sendRequest(const json& request);

private:
    SOCKET clientSocket;
    bool isConnected;
};
