//
// Created by hq on 2023/7/24.
//

#ifndef IOMODEL_MPST_H
#define IOMODEL_MPST_H
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void client_handler(int clifd) {
    // ????????
    char buf[1024];
    ssize_t bytesRead = read(clifd, buf, sizeof(buf));
    if (bytesRead == -1) {
        // ??????
        perror("read");
    } else if (bytesRead == 0) {
        // ????????
        close(clifd);
    } else {
        // ??????????
        // dosomethingonbuf(buf);

        // ????????
        ssize_t bytesWritten = write(clifd, buf, bytesRead);
        if (bytesWritten == -1) {
            // ??????
            perror("write");
        }
        close(clifd);
    }
}

/*
 * Multiprocess single thread
 * */
int mpst(int argc, char* argv[]) {

    int port;
    port = std::stoi(argv[1]);
    std::cerr << "port:"<<port<<"\n";
    std::cerr << "htons(port):"<<htons(port)<<"\n";

    int srvfd = socket(AF_INET, SOCK_STREAM, 0);
    if (srvfd == -1) {
        perror("socket");
        return 1;
    }

    // ??????????
    struct sockaddr_in srvAddr;
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvAddr.sin_port = htons(port);
    if (bind(srvfd, reinterpret_cast<struct sockaddr*>(&srvAddr), sizeof(srvAddr)) == -1) {
        perror("bind");
        close(srvfd);
        return 1;
    }

    if (listen(srvfd, 10) == -1) {
        perror("listen");
        close(srvfd);
        return 1;
    }

    while (true) {
        int clifd = accept(srvfd, nullptr, nullptr);
        if (clifd == -1) {
            perror("accept");
            close(srvfd);
            return 1;
        }

        int ret = fork();
        switch (ret) {
            case -1:
                // ????????????
                perror("fork");
                close(clifd);
                break;
            case 0:
                // ???
                close(srvfd);
                client_handler(clifd);
                return 0;
            default:
                // ???
                close(clifd);
                break;
        }
    }

    close(srvfd);
    return 0;
}

#endif //IOMODEL_MPST_H
