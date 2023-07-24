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

    /*
     int kq = kqueue();：创建一个 kqueue 实例，并将其返回的文件描述符保存在 kq 变量中。
     kqueue 是一个用于事件驱动编程的机制，它可以监视多个文件描述符上发生的事件。
    */
    int kq = kqueue();

    /*
          定义了两个变量，ev 和 events。ev 是用于描述事件的 struct kevent 结构体变量，
          events 是一个 struct kevent 类型的数组，用于存储触发的事件。
    */
    struct kevent ev, events[EPOLL_SIZE] = {0};

    /*
         使用 EV_SET 宏初始化 ev，并设置以下参数：
        sockfd：要监听的文件描述符。
        EVFILT_READ：要监听的事件类型，这里是读事件。
        EV_ADD：标记要添加监听的事件。
        其他参数为0或NULL。
    */
    EV_SET(&ev, sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);

    /*
         使用 kevent 函数将事件 ev 添加到 kqueue 实例 kq 中进行监听。
         这里的监听操作是具体地触发读事件。
    */
    kevent(kq, &ev, 1, NULL, 0, NULL);

//    EV_SET(&ev, clientfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
//    kevent(kq, &ev, 1, NULL, 0, NULL);
    while (1)
    {
        /*
        kevent 函数的调用，用于监听事件并获取已触发的事件。
        等待并获取已触发的事件，并将其存储在 events 数组中，nready 则表示触发的事件数量。
            kq：kqueue 实例的文件描述符。
            NULL：表示不关心触发的事件。
            0：表示忽略特殊的事件筛选标志。
            events：事件数组，用于存储已触发的事件。
            EPOLL_SIZE：events 数组的大小，指定了最多可以存储多少个事件。
            NULL：kevent 函数的超时参数，这里设置为无超时。
            nready：表示触发的事件数量，可以根据 nready 的值来遍历 events 数组，并处理对应的事件。
        */
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
