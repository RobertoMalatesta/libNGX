#include "Core/Core.h"

using namespace std;
using namespace ngx;
using namespace ngx::Core;

int PoolTest() {

    Pool pool;

    cout<<"Pool Allocate Start..."<<endl;

    void *p1 = pool.Allocate(100);
    cout<<"Pool Allocate p1="<<p1<<endl;
    pool.Free(p1);

    p1 = pool.Allocate(1000);
    cout<<"Pool Allocate p2="<<p1<<endl;
    pool.Free(p1);

    p1 = pool.Allocate(10000);
    cout<<"Pool Allocate p3="<<p1<<endl;
    pool.Free(p1);

    cout<<"Pool Allocate Done!"<<endl;

    cout<<"Pool GC Test Start!"<<endl;


    vector<void *> PointerList;

    PointerList.clear();

    for (int i = 0; i < 500; i++) {
        void *pData = pool.Allocate((size_t)i);

        if (i % 2 == 0) {
            pool.Free(pData);
        }
        else {
            PointerList.push_back(pData);
        }
    }
    pool.GC();

    for (auto it = PointerList.begin(); it != PointerList.end(); it++){
        pool.Free(*it);
    }

    pool.GC();

    cout<<"Pool GC Test Done!"<<endl;

    return 0;
}
