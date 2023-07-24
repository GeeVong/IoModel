#include <iostream>
#include "../models/poll/mac_poll.h"
#include "../models/select/mac_select.h"
#include "../models/epoll/kqueue.h"
#include "../models/spst/spst.h"
#include "../models/mpst/mpst.h"

enum IOModelType{
    IOModelType_spst = 0,
    IOModelType_mpst = 1,
    IOModelType_spmt = 2,

    IOModelType_select = 3,
    IOModelType_poll = 4,
    IOModelType_epoll = 5,
};

int main(int argc, char* argv[]) {
    int port;
    port = std::stoi(argv[1]);
    std::cerr << "port:"<<port<<"\n";
    std::cerr << "htons(port):"<<htons(port)<<"\n";

    int expression = std::stoi(argv[2]);
    switch (expression) {
        case IOModelType_spst: // 短时间哪大量数据，完全承受不住单个client
            spst(argc, argv);
        case IOModelType_mpst: // todo
            mpst(argc, argv);
        case IOModelType_spmt: // todo
            mpst(argc, argv);
        case IOModelType_epoll:
            kqueue_darwin(argc, argv);
        case IOModelType_select:
            select_darwin(argc, argv);
        case IOModelType_poll:
            poll_darwin(argc, argv);
    }
    return 0;
}
