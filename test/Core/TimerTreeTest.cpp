#include "Core/Core.h"
#include <unistd.h>
#include <iostream>

using namespace std;

using namespace ngx::Core;


static void func(void *, ThreadPool *) {

    char c[50];
    WriteHTTPTime(c, HTTPTimeSize);
    printf("Timer function: %s\n", c);
}

int TimerTreeTest() {

    Pool MemAllocator;
    ThreadPool T(7);
    TimerTree Tree(&MemAllocator);
    TimeModuleInit();

//    Tree.PostTimerPromise(1, func, nullptr);
//    Tree.PostTimerPromise(3, func, nullptr);
//    Tree.PostTimerPromise(5, func, nullptr);
//    Tree.PostTimerPromise(10, func, nullptr);
//    Tree.PostTimerPromise(15, func, nullptr);
//    Tree.PostTimerPromise(18, func, nullptr);

    int i = 0;

    while (i++ < 20) {
        Tree.QueueExpiredTimer(&T);
        usleep(1000 * 1000);
    }
    return 0;
}
