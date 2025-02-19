#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char* argv[]) {
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create server socket: " << std::endl;
        return 1;
    }

    // Since the tester restarts your program quite often, setting SO_REUSEADDR
    // ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        close(server_fd);
        std::cerr << "setsockopt failed: " << std::endl;
        return 1;
    }

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9092);

    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) != 0) {
        close(server_fd);
        std::cerr << "Failed to bind to port 9092" << std::endl;
        return 1;
    }

    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        close(server_fd);
        std::cerr << "listen failed" << std::endl;
        return 1;
    }

    std::cout << "Waiting for a client to connect...\n";

    struct sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);
     
    int client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
    std::cout << "Client connected\n";
    const int MAX_BUFFER_SIZE = 4096;
    char buffer[MAX_BUFFER_SIZE];
    int rv = recv(client_fd, buffer, MAX_BUFFER_SIZE, 0);
    int32_t message_size = htonl(0);
    int32_t correlation_id;
    //_______________________________________________________
    //                  Alternative method
    //_______________________________________________________
    // struct KafkaRequestHeader {
    //     int32_t message_size;
    //     int16_t request_api_key;
    //     int16_t request_api_version;
    //     int32_t correlation_id;
    // };
    // KafkaRequestHeader header;
    // int rv = recv(client_fd, &header, sizeof(header), 0);
    //_______________________________________________________

    /*
    Field	            Data type	        Description
    message_size        INT32               size of this request
    request_api_key	    INT16	            The API key for the request
    request_api_version	INT16	            The version of the API for the request
    correlation_id	    INT32	            A unique identifier for the request
    client_id	        NULLABLE_STRING	    The client ID for the request
    TAG_BUFFER	        COMPACT_ARRAY	    Optional tagged fields
    */

    //Skips first 8 bytes
    std::memcpy(&correlation_id, buffer + 8, sizeof(correlation_id));
    correlation_id = ntohl(correlation_id);

    send(client_fd, &message_size, sizeof(message_size), 0);
    
    correlation_id = htonl(correlation_id);
    send(client_fd, &correlation_id, sizeof(correlation_id), 0);

    close(client_fd);

    close(server_fd);
    return 0;
}