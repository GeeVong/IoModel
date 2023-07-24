#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <errno.h>

#define BUF_LEN 1024

int main(int argc, char *argv[])
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

	fd_set rfds, rset;

	FD_ZERO(&rfds);
	FD_SET(sockfd, &rfds);

	int maxfd = sockfd + 1;
	while (1)
	{
		rset = rfds;
		int nready = select(maxfd, &rset, NULL, NULL, NULL);
		if (nready < 0)
		{
			continue;
		}

		if (FD_ISSET(sockfd, &rset))
		{
			struct sockaddr_in clientaddr;
			socklen_t client_len = sizeof(clientaddr);

			int clientfd = accept(sockfd, (struct sockaddr*)&clientaddr, &client_len);
			if (clientfd <= 0)
			{
				continue;
			}

			FD_SET(clientfd, &rfds);

			if (clientfd >= maxfd)
			{
				maxfd = clientfd + 1;
			}

			printf("sockfd %d, maxfd %d, clientfd %d\n", sockfd, maxfd, clientfd);
			if (--nready == 0)
			{
				continue;
			}
		}

		int i = 0;
		for (i = sockfd + 1; i < maxfd; i++)
		{
			if (FD_ISSET(i, &rset))
			{
				char buffer[BUF_LEN] = {0};
				int ret = recv(i, buffer, BUF_LEN, 0);
				if (ret < 0)
				{
					if ((EAGAIN == errno) || (EWOULDBLOCK == errno))
					{
						printf("recv all data\n");
					}
					else
					{
						FD_CLR(i, &rfds);
						close(i);

						if (i == maxfd - 1)
						{
							maxfd--;
						}
						continue;
					}
				}
				else if (0 == ret)
				{
					printf("client close\n");
					FD_CLR(i, &rfds);
					close(i);
					if (i == maxfd - 1)
					{
						maxfd--;
					}
					continue;
				}
				else
				{
					printf("recv %d byte data, content is [%s]\n", ret, buffer);
				}

				if (--nready == 0) break;
			}
		}
	}

	return 0;
}

