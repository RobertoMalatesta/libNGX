#include "Core/Core.h"

using namespace std;
using namespace ngx;
using namespace ngx::Core;

const int sizeRound = 1000;

int NoPool() {

    vector<void *> PointerList;

    for (int i = 0; i < sizeRound; i++) { // Allocate 500 objects and free half of them
        void *pData = malloc((size_t) 200);

        if (i % 2 == 0) {
            ::free(pData);
        } else {
            PointerList.push_back(pData);
        }
    }

    for (auto &it : PointerList) { // Free all the rest objects
        ::free(it);
    }

    PointerList.clear();
    return 0;
}

int PoolTest1() {

    Pool pool;
    vector<void *> PointerList;

    for (int i = 0; i < sizeRound; i++) { // Allocate 500 objects and free half of them
        void *pData = pool.allocate((size_t) 200);

        if (i % 2 == 0) {
            pool.free(pData);
        } else {
            PointerList.push_back(pData);
        }
    }

    pool.GC();  // Call GC to destroy free blocks

    for (auto &it : PointerList) { // Free all the rest objects
        pool.free(it);
    }

    pool.GC();  // Call GC to destroy free blocks, now the Pool returns to the origin state
    PointerList.clear();

    pool.Reset();
    pool.GC();  // GC all free blocks

    return 0;
}

int PoolTests() {

    clock_t clock1 = clock();
    for(int i=0; i<1000; i++)
        NoPool();
    printf("no pool: %ld\n", (clock() -clock1)/1000);
    clock1 = clock();
    for(int i=0; i<1000; i++)
        PoolTest1();
    printf("pool impl: %ld\n", (clock() -clock1)/1000);
    return 0;
}
