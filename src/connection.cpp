#include "connection.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

int Connection::receiveMessage(int client_fd, std::vector<uint8_t>& buffer) {
    int rv = recv(client_fd, buffer.data(), buffer.size(), 0);
    if (rv < 0) {
        std::cerr << "Failed to receive data" << std::endl;
    }
    return rv;
}

void Connection::sendResponse(int client_fd, const ResponseMessage& response) {
    std::vector<uint8_t> buffer = response.serialize();

    if (send(client_fd, buffer.data(), buffer.size(), 0) < 0) {
        std::cerr << "Failed to send response" << std::endl;
    }
}
