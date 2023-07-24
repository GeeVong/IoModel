#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>


int sendCount = 0;

void coutMessage(){
    std::cout << "客户端发包数量:"<<sendCount;
    std::cout<<"\n";
}

int main(int argc, char* argv[]) {
    int port;
    port = std::stoi(argv[1]);
    std::cerr << "port:"<<port<<"\n";
    std::cerr << "htons(port):"<<htons(port)<<"\n";


    // 创建客户端套接字
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "无法创建套接字\n";
        return -1;
    }

    // 设置服务器地址和端口
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // 替换为实际的服务器端口
    inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)); // 替换为实际的服务器地址

    // 连接到服务器
    if (connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "连接失败\n";
        return -1;
    }

    if (argv[2] != 0 ){
        while (true){
            /* 发送数据给服务器
             * 脚本测试
             *  spsm 模式无法承载
             */
            if (send(clientSocket, argv[2], strlen(argv[2]), 0) < 0) {
                std::cerr << "发送数据失败\n";
                return -1;
            }
            sendCount++;

            // 检查是否退出
            if (strcmp(argv[2], "quit") == 0) {
                std::cout << "退出程序\n";
                break;
            }
        }

    } else{
        /*
         * 单线程模式发包：
         *  接收用户输入并发送给服务器
         *
         * */
        char input[4096];
        std::cout << "请输入要发送的数据 (输入'quit'退出)：\n";
        while (true) {
            std::cin.getline(input, sizeof(input));

            std::string quit = "quit";
            if (quit.compare(input) == 0) {
                coutMessage();
                // 关闭套接字
                close(clientSocket);
            }

            /*
             *           发送数据给服务器
             * 客户端关闭
             *      socket close(clientSocket); 还会send一个包过去服务器
             * */
            if (send(clientSocket, input, strlen(input), 0) < 0) {
                std::cerr << "发送数据失败\n";
                return -1;
            }
            sendCount++;
        }
    }

    // 关闭套接字
    close(clientSocket);
    return 0;
}

