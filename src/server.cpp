#include "server.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

Server::Server(int port) : port(port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        throw std::runtime_error("Failed to create server socket");
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        close(server_fd);
        throw std::runtime_error("setsockopt failed");
    }
}

Server::~Server() {
    close(server_fd);
}

int Server::start() {
    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) != 0) {
        close(server_fd);
        throw std::runtime_error("Failed to bind to port");
    }
    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        close(server_fd);
        throw std::runtime_error("Listen failed");
    }

    std::cout << "Server listening on port " << port << std::endl;
    return server_fd;
}

int Server::acceptClient(struct sockaddr_in &client_addr) {
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
    if (client_fd < 0) {
        throw std::runtime_error("Failed to accept client connection");
    }
    std::cout << "Client connected" << std::endl;
    return client_fd;
}
