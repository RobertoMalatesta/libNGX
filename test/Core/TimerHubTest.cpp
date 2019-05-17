#include "Core/Core.h"
#include <unistd.h>
#include <iostream>

using namespace std;

using namespace ngx::Core::Support;

static void func(void *, ThreadPool *) {

    char c[50];
    WriteHTTPTime(c, HTTP_TIME_SIZE);
    printf("Timer function: %s\n", c);
}

int TimerTreeTest() {

    Pool MemAllocator;
    ThreadPool T(7);
    SocketTimerHub Tree;
    TimeModuleInit();

//    Tree.PostTimerJob(1, func, nullptr);
//    Tree.PostTimerJob(3, func, nullptr);
//    Tree.PostTimerJob(5, func, nullptr);
//    Tree.PostTimerJob(10, func, nullptr);
//    Tree.PostTimerJob(15, func, nullptr);
//    Tree.PostTimerJob(18, func, nullptr);

    int i = 0;

    while (i++ < 20) {
//        Tree.QueueExpiredTimer(&T);
        ForceSleep(NANO_SECOND_SIZE);
    }
    return 0;
}
