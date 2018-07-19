#include "Core/Core.h"
#include <unistd.h>
#include <iostream>

using namespace std;

using namespace ngx::Core;

Promise *func(Promise *, void * args) {
    printf("Doing Promise: %ld\n", (long)args);
    return nullptr;
}

int ThreadPoolTest()  {

    Pool pool;
    ThreadPool TPool(&pool, 8);
    TPool.Start();

    for(int i=0; i< 1000; i++) {    // [BUG] 内存分配器有BUG
        Promise::PostPromise(&TPool, func, (void *)i);
        usleep(10);
    }

    TPool.Stop();
    return 0;
}