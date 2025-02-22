#include "server.hpp"
#include "connection.hpp"
#include "protocol.hpp"
#include <iostream>
#include <unistd.h>

const short PORT = 9092;
const short MAX_BUFFER_SIZE = 4096;

int main()
{
    try
    {
        Server server(PORT);
        server.start();

        struct sockaddr_in client_addr{};
        
        // while (1)
        // {
            int client_fd = server.acceptClient(client_addr);
            std::vector<uint8_t> buffer(MAX_BUFFER_SIZE);

            if (Connection::receiveMessage(client_fd, buffer) > 0)
            {
                RequestMessage request = RequestMessage::parse(buffer);

                ResponseMessage response{};
                response.correlation_id = request.correlation_id;
                response.error_code = (request.api_version < 0 | request.api_version > 4 )? 35 : 0; 
                response.api_keys = 2;
                response.api_key = 18;
                response.min_version = 0;
                response.max_version = 4;
                response.throttle_time_ms = 0;

                Connection::sendResponse(client_fd, response);
                close(client_fd);
            }
        // }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
