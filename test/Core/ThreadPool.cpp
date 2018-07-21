#include "Core/Core.h"
#include <unistd.h>
#include <iostream>

using namespace std;

using namespace ngx::Core;

Promise *func(Promise *, void *args) {
    printf("Doing Promise: %ld\n", (long) args);
    return nullptr;
}

int ThreadPoolTest() {

    ThreadPool T(8);

    for(long i=0; i < 2000000; i++) {
        T.PostPromise(func, (void *)i);
    }
    usleep( 20 * 1000 * 1000);

    return 0;
}