#include "Core/Core.h"
#include <unistd.h>
#include <iostream>

using namespace std;

using namespace ngx::Core;

Promise *func(Promise *, void * args) {
    cout<< "doing Promise"<<endl;
    return nullptr;
}

int ThreadPoolTest()  {

    Pool pool;
    ThreadPool TPool(&pool, 8);
    TPool.Start();

    for(int i=0; i< 100; i++) {
        Promise::PostPromise(&TPool, func, nullptr);
        usleep(10);
    }

    usleep(5000000);
    cout<< "release"<<endl;
    TPool.Stop();
    return 0;
}