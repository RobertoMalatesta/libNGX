#include "Core/Core.h"

using namespace std;
using namespace ngx;
using namespace ngx::Core;

int PoolTest() {

    Pool pool;

    cout<<"Pool Class Test Start..."<<endl;

    cout<<"Pool Allocate Start..."<<endl;

    void *p1 = pool.Allocate(100);          // Test if Pool can allocate small object
    cout<<"Pool Allocate p1="<<p1<<endl;
    pool.Free(&p1);

    p1 = pool.Allocate(1000);               // Test if Pool can allocate medium object
    cout<<"Pool Allocate p2="<<p1<<endl;
    pool.Free(&p1);

    p1 = pool.Allocate(10000);              // Test if Pool can allocate big object
    cout<<"Pool Allocate p3="<<p1<<endl;
    pool.Free(&p1);

    cout<<"Pool Allocate Done!"<<endl;

    cout<<"Pool GC Test Start!"<<endl;

    vector<void *> PointerList;

    PointerList.clear();

    for (int i = 0; i < 500; i++) { // Allocate 500 objects and free half of them
        void *pData = pool.Allocate((size_t)i);

        if (i % 2 == 0) {
            pool.Free(&pData);
        }
        else {
            PointerList.push_back(pData);
        }
    }
    pool.GC();  // Call GC to destroy free blocks

    for (auto it = PointerList.begin(); it != PointerList.end(); it++){ // Free all the rest objects
        pool.Free(&(*it));
    }

    pool.GC();  // Call GC to destroy free blocks, now the Pool returns to the origin state
    PointerList.clear();

    for (int i = 0; i < 500; i++) { // Allocate 500 objects and free half of them
        void *pData = pool.Allocate((size_t)i);

        if (i % 2 == 0) {
            pool.Free(&pData);
        }
        else {
            PointerList.push_back(pData);
        }
    }
    pool.GC();  // Allocate 500 objects and free half of them

    p1 = pool.Allocate(1000);   // Try to allocate a new object;
    cout<<"Pool Allocate after GC p4="<<p1<<endl;
    pool.Free(&p1);

    for (auto it = PointerList.begin(); it != PointerList.end(); it++){ // Free all object
        pool.Free(&(*it));
    }

    pool.GC();  // GC all free blocks

    cout<<"Pool GC Test Done!"<<endl;

    return 0;
}
