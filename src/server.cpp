#include "server.hpp"
#include "connection.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        throw std::runtime_error("Failed to create server socket.");
    }

    int reuse = {1};
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        close(server_fd);
        throw std::runtime_error("setsockopt failed.");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) != 0) {
        close(server_fd);
        throw std::runtime_error("Failed to bind to port.");
    }
    backlog = {5};
    if (listen(server_fd, backlog) != 0) {
        close(server_fd);
        throw std::runtime_error("listen failed.");
    }
}

Server::~Server() {
    close(server_fd);
}

void Server::start() {
    std::cout << "Server listening on port " << ntohs(server_addr.sin_port) << "\n";

    while (true) {
        struct sockaddr_in client_addr{};
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);

        if (client_fd < 0) {
            std::cerr << "Failed to accept client connection.\n";
            continue;
        }

        std::cout << "Client connected. Spawning a thread...\n";
        
        // Creates a new thread to handle each client
        std::jthread client_thread(&Connection::handleClient, client_fd);
        client_thread.detach();
    }
}
