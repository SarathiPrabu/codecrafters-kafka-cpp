#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <vector>
#include <cstdint>
#include "protocol.hpp"

class Connection {
public:
    static int receiveMessage(int client_fd, std::vector<uint8_t>& buffer);
    static void sendResponse(int client_fd, const ResponseMessage& response);
};

#endif // CONNECTION_HPP
