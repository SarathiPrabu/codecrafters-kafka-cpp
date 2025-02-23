#include "connection.hpp"
#include "protocol.hpp" 
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <cstring>

#define MAX_BUFFER_SIZE 4096

void Connection::handleClient(int client_fd) {
    std::cout << "Handling client in thread...\n";
    std::vector<uint8_t> buffer(MAX_BUFFER_SIZE);

    while(Connection::receiveMessage(client_fd, buffer) > 0) 
    {
        //Parse request using RequestMessage
        RequestMessage request = RequestMessage::parse(buffer);

        //Prepare response using ResponseMessage
        ResponseMessage response{};
        response.correlation_id = request.correlation_id;
        response.error_code = (request.api_version < 0 || request.api_version > 4) ? 35 : 0;
        response.api_keys.push_back(APIKeys(18, 0, 4));  // API_VERSIONS
        response.api_keys.push_back(APIKeys(75, 0, 0));  // New API Key 75
        response.throttle_time_ms = 0;

        //Send response
        Connection::sendResponse(client_fd, response);
    }

    std::cout << "Closing connection for client.\n";
    close(client_fd);
}

long Connection::receiveMessage(int client_fd, std::vector<uint8_t>& buffer) {
    return recv(client_fd, buffer.data(), buffer.size(), 0);
}

void Connection::sendResponse(int client_fd, const ResponseMessage& response) {
    //Serialize response into binary format
    std::vector<uint8_t> buffer = response.serialize();

    //Send serialized buffer
    long bytes_sent = send(client_fd, buffer.data(), buffer.size(), 0);

    if (bytes_sent < 0) {
        std::cerr << "Failed to send response" << std::endl;
    } else {
        std::cout << "Sent " << bytes_sent << " bytes to client" << std::endl;
    }
}
