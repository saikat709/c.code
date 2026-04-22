#pragma once

#include <cstdint>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Send a message with a 4-byte big-endian length prefix so the receiver
// always knows exactly how many bytes to read.  Returns false on error.
inline bool sendFramed(int fd, const std::string& data) {
    uint32_t len = htonl(static_cast<uint32_t>(data.size()));
    // Send length prefix
    const char* lenPtr = reinterpret_cast<const char*>(&len);
    size_t sent = 0;
    while (sent < sizeof(len)) {
        ssize_t n = send(fd, lenPtr + sent, sizeof(len) - sent, 0);
        if (n <= 0) return false;
        sent += n;
    }
    // Send payload
    sent = 0;
    while (sent < data.size()) {
        ssize_t n = send(fd, data.c_str() + sent, data.size() - sent, 0);
        if (n <= 0) return false;
        sent += n;
    }
    return true;
}

// Receive a framed message.  Blocks until all bytes arrive.
// Returns false if the connection closed or an error occurred.
inline bool recvFramed(int fd, std::string& out) {
    // Read 4-byte length prefix
    uint32_t netLen = 0;
    char* lenPtr = reinterpret_cast<char*>(&netLen);
    size_t received = 0;
    while (received < sizeof(netLen)) {
        ssize_t n = recv(fd, lenPtr + received, sizeof(netLen) - received, 0);
        if (n <= 0) return false;
        received += n;
    }
    uint32_t msgLen = ntohl(netLen);
    if (msgLen == 0) { out.clear(); return true; }

    // Read payload
    out.resize(msgLen);
    received = 0;
    while (received < msgLen) {
        ssize_t n = recv(fd, &out[received], msgLen - received, 0);
        if (n <= 0) return false;
        received += n;
    }
    return true;
}
