#include <iostream>
#include "poll/mac_poll.h"
#include "select/mac_select.h"
#include "epoll/kqueue.h"

int main() {
    char *argv[] = {"argument1", "8099", "argument3"}; // 模拟参数数组

    int argc = sizeof(argv) / sizeof(argv[0]); // 计算参数数量

    //select_darwin(argc, argv);
    //poll_darwin(argc, argv);
    kqueue_darwin(argc, argv);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
