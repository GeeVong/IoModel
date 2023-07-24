#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    // 创建客户端套接字
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "无法创建套接字\n";
        return -1;
    }

    // 设置服务器地址和端口
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8099); // 替换为实际的服务器端口
    inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)); // 替换为实际的服务器地址

    // 连接到服务器
    if (connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "连接失败\n";
        return -1;
    }

    // 接收用户输入并发送给服务器
    char input[4096];
    std::cout << "请输入要发送的数据 (输入'quit'退出)：\n";
    while (true) {
        std::cin.getline(input, sizeof(input));

        // 发送数据给服务器
        if (send(clientSocket, input, strlen(input), 0) < 0) {
            std::cerr << "发送数据失败\n";
            return -1;
        }

        // 检查是否退出
        if (strcmp(input, "quit") == 0) {
            std::cout << "退出程序\n";
            break;
        }
    }

    // 关闭套接字
    close(clientSocket);

    return 0;
}
