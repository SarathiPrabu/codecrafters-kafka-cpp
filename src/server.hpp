#pragma once

#include <netinet/in.h>

class Server {
public:
    explicit Server(int port);
    ~Server();
    
    int start();
    int acceptClient(struct sockaddr_in &client_addr);

private:
    int server_fd;
    int port;
};
