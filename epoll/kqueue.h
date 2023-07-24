//
// Created by hq on 2023/7/24.
//

#ifndef IOMODEL_KQUEUE_H
#define IOMODEL_KQUEUE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

#include <sys/event.h>
#include <unistd.h>



#define BUF_LEN 1024
#define EPOLL_SIZE 1024


//int main(int argc, char *argv[])
int kqueue_darwin(int argc, char *argv[])
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

    int kq = kqueue();
    struct kevent ev, events[EPOLL_SIZE] = {0};

    EV_SET(&ev, sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(kq, &ev, 1, NULL, 0, NULL);

    while (1)
    {
        int nready = kevent(kq, NULL, 0, events, EPOLL_SIZE, NULL);
        if (nready == -1)
        {
            printf("kevent\n");
            break;
        }

        int i = 0;
        for (i = 0; i < nready; i++)
        {
            if (events[i].ident == sockfd)
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
                printf("recvived from %s at port %d, sockfd:%d, clientfd:%d\n",
                       inet_ntop(AF_INET, &client_addr.sin_addr, str, sizeof(str)),
                       ntohs(client_addr.sin_port),
                       sockfd,
                       clientfd);

                EV_SET(&ev, clientfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                kevent(kq, &ev, 1, NULL, 0, NULL);
            }
            else
            {
                int clientfd = events[i].ident;
                char buffer[BUF_LEN] = {0};
                int ret = recv(clientfd, buffer, BUF_LEN, 0);
                if (ret < 0)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        printf("read all data\n");
                    }
                    else
                    {
                        close(clientfd);
                        EV_SET(&ev, clientfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                        kevent(kq, &ev, 1, NULL, 0, NULL);
                        continue;
                    }

                }
                else if (ret == 0)
                {
                    printf("disconnect %d\n", clientfd);

                    close(clientfd);
                    EV_SET(&ev, clientfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(kq, &ev, 1, NULL, 0, NULL);
                    continue;
                } else
                {
                    printf("recv: %s, %d Bytes\n", buffer, ret);
                }
            }
        }


//        char input[4096];
//        std::cout << "????????? (??'quit'??)?\n";
//
//        // ??????
//        if (strcmp(input, "quit") == 0) {
//            std::cerr << "??????\n";
//            break;
//        }
    }

    return 0;
}

#endif //IOMODEL_KQUEUE_H
