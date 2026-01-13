#pragma once

#define NOMINMAX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include "json.hpp"

class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    bool connectToServer(const std::string& ip, int port);
    json sendRequest(const json& request);

private:
    int clientSocket;
    bool isConnected;
};
