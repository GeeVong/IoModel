//
// Created by hq on 2023/7/24.
//

#ifndef IOMODEL_SPST_H
#define IOMODEL_SPST_H
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/*
 * single process single thread model
 * */

int spst(int argc, char* argv[]) {
    int port;
    port = std::stoi(argv[1]);
    std::cerr << "port:"<<port<<"\n";
    std::cerr << "htons(port):"<<htons(port)<<"\n";
    // ?????
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create server socket" << std::endl;
        return 1;
    }

    // ?? IP ???
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Failed to bind server socket" << std::endl;
        close(server_fd);
        return 1;
    }

    // ????
    if (listen(server_fd, 10) == -1) {
        std::cerr << "Failed to listen on server socket" << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Server is listening on port 8080..." << std::endl;

    while (true) {
        // ???????
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd == -1) {
            std::cerr << "Failed to accept client connection" << std::endl;
            close(server_fd);
            return 1;
        }

        // ??????????
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        ssize_t num_bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (num_bytes == -1) {
            std::cerr << "Failed to read data from client" << std::endl;
            close(client_fd);
            close(server_fd);
            return 1;
        }

        std::cout << "Received data from client: " << buffer << std::endl;

        // ????????
        const char *response = "Hello from server!";
        if (send(client_fd, response, strlen(response), 0) == -1) {
            std::cerr << "Failed to send response to client" << std::endl;
            close(client_fd);
            close(server_fd);
            return 1;
        }

        // ?????????
        close(client_fd);
    }

    // ????????
    close(server_fd);

    return 0;
}

#endif //IOMODEL_SPST_H
