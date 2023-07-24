//
// Created by hq on 2023/7/24.
//

#ifndef IOMODEL_MAC_POLL_H
#define IOMODEL_MAC_POLL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h> // ????
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/poll.h>
#include "unistd.h"

#define BUF_LEN 1024
#define POLL_SIZE 1024

int poll_darwin(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("param port error\n");
        return -1;
    }

    int port = atoi(argv[1]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("bind\n");
        return -2;
    }

    if (listen(sockfd, 5) < 0)
    {
        perror("listen\n");
    }

    struct pollfd fds[POLL_SIZE] = {0};
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;

    int maxfd = 1, i = 0;
    for (i = 1; i < POLL_SIZE; i++)
    {
        fds[i].fd = -1;
    }

    while (1)
    {
        int nready = poll(fds, maxfd, 5);
        if (nready <= 0) continue;

        if (POLLIN == (fds[0].revents & POLLIN))
        {
            struct sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(struct sockaddr_in));
            socklen_t client_len = sizeof(client_addr);

            int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
            if (0 > clientfd)
            {
                continue;
            }

            char str[INET_ADDRSTRLEN] = {0};
            printf("received from %s at port %d, sockfd:%d, clientfd:%d\n",
                   inet_ntop(AF_INET, &client_addr.sin_addr, str, sizeof(str)),
                   ntohs(client_addr.sin_port),
                   sockfd,
                   clientfd);

            fds[clientfd].fd = clientfd;
            fds[clientfd].events = POLLIN;

            if (clientfd >= maxfd)
            {
                maxfd = clientfd + 1;
            }

            if (--nready == 0) continue;
        }

        for (i = 1;i < maxfd; i++)
        {
            if (fds[i].revents & (POLLIN|POLLERR))
            {
                char buffer[BUF_LEN] = {0};
                int ret = recv(i, buffer, BUF_LEN, 0);
                if (ret < 0)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        printf("read all data\n");
                    }
                    else
                    {
                        close(i);
                        fds[i].fd = -1;
                        if (i == maxfd - 1)
                        {
                            maxfd--;
                        }
                        continue;
                    }

                }
                else if (ret == 0)
                {
                    printf("disconnected %d\n", i);

                    close(i);
                    fds[i].fd = -1;
                    if (i == maxfd - 1)
                    {
                        maxfd--;
                    }
                    continue;
                } else
                {
                    printf("recv: %s, %d Bytes\n", buffer, ret);
                }
                if (--nready == 0) break;
            }
        }
    }

    return 0;
}


#endif //IOMODEL_MAC_POLL_H
