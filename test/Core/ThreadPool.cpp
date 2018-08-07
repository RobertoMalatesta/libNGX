#include "Core/Core.h"
#include <unistd.h>
#include <iostream>

using namespace std;

using namespace ngx::Core;

atomic_uint64_t clocks = 1;
atomic_uint64_t turns = 1;

void func(void *, ThreadPool *) {

    int j=0;

    clock_t t1 = clock(), t2;

    for(int i =0; i< 10000; i++) {
        j+=i;
    }

    t2 = clock() - t1;

    long ratio = (clocks+=t2) / turns++;

    printf("Turn Ratio: %ld\n", ratio);
}

int ThreadPoolTest() {

    ThreadPool T(7);

    for(long i=0; i < 500000; i++) {
        T.PostPromise(func, (void *)i);
    }

    ForceUSleep( 10 * 1000 * 1000);

    return 0;
}
