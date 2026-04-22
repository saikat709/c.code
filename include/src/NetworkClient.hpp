#pragma once

#define NOMINMAX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include <string>
#include <iostream>
#include "json.hpp"

using namespace std;

class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    bool connectToServer(const string& ip, int port);
    json sendRequest(const json& request);
    vector<json> getPendingNotifications();

private:
    int clientSocket;
    bool isConnected;
    bool stopListener;
    thread listenerThread;

    mutex sendMutex;
    mutex pendingMutex;
    mutex responseMutex;
    condition_variable responseCv;

    vector<json> pendingNotifications;
    queue<json> pendingResponses;

    void listenForBroadcasts();
};
