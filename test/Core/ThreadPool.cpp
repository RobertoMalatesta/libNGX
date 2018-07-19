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

    for(long i=0; i< 500; i++) {    // [BUG] 内存分配器有BUG
        TPool.PostPromise(func, (void *)i);
        usleep(10);
    }

    usleep(1000000);

    TPool.Stop();
    return 0;
}