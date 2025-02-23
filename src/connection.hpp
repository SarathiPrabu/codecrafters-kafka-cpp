#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "protocol.hpp"

#include <vector>
#include <cstdint>

class Connection {
public:
    static void handleClient(int client_fd);
    static long receiveMessage(int client_fd, std::vector<uint8_t>& buffer);
    static void sendResponse(int client_fd, const ResponseMessage& response);
};

#endif // CONNECTION_HPP
