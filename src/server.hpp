#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <thread>

class Server {
private:
    int server_fd;
    struct sockaddr_in server_addr;
    int backlog;

public:
    explicit Server(int port);
    ~Server();
    void start();  // Accepts connections & spawns threads
};

#endif // SERVER_HPP
