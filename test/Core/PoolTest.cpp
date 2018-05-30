#include <iostream>

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

    return 0;
}
