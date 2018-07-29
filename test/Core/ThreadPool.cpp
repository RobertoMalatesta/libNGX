#include "Core/Core.h"
#include <unistd.h>
#include <iostream>

using namespace std;

using namespace ngx::Core;

atomic_int clocks = 1;
atomic_int turns = 1;

Promise *func(Promise *, void *) {

    int j=0;

    clock_t t1 = clock(), t2;

    for(int i =0; i< 100000; i++) {
        j+=i;
    }

    t2 = clock() - t1;

    long ratio = (clocks+=t2) / turns++;

    printf("Turn Ratio: %ld\n", ratio);
    return nullptr;
}

int ThreadPoolTest() {

    ThreadPool T(7);

    for(long i=0; i < 2000000; i++) {
        T.PostPromise(func, (void *)i);
    }

    usleep( 40 * 1000 * 1000);

    return 0;
}