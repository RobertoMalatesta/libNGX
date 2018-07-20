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

    for(long i=0; i< 100000; i++) {
        TPool.PostPromise(func, (void *)i);
    }
    usleep(10*1000*1000);
    TPool.Stop();

    return 0;
}