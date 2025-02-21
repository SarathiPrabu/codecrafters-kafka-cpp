#include "server.hpp"
#include "connection.hpp"
#include "protocol.hpp"
#include <iostream>
#include <unistd.h>

int main() {
    try {
        Server server(9092);
        server.start();

        struct sockaddr_in client_addr{};
        int client_fd = server.acceptClient(client_addr);

        const int MAX_BUFFER_SIZE = 4096;
        std::vector<uint8_t> buffer(MAX_BUFFER_SIZE);

        if (Connection::receiveMessage(client_fd, buffer) > 0) {
            RequestMessage request = RequestMessage::parse(buffer);

            // Prepare API_VERSIONS response
            ResponseMessage response{};
            response.correlation_id = request.correlation_id;
            response.error_code = 0;  // No error
            response.api_versions_count = 1;
            response.api_key = 18;  // API_VERSIONS key
            response.min_version = 0;
            response.max_version = 4;  // Ensure MaxVersion ≥ 4
            response.throttle_time_ms = 0;
            response.no_tags = 0;
            response.api_key_tags =0;

            // Send response
            Connection::sendResponse(client_fd, response);
        }

        close(client_fd);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
