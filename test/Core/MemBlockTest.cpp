#include "Core/Core.h"

using namespace std;
using namespace ngx;
using namespace ngx::Core;

int MemBlockTest() {

    cout<<"MemBlock Class Test Start!"<<endl;

	MemBlock *memBlk = MemBlock::New(4096);

    cout<<"MemBlockTest AddressToMemBlock Start..."<<endl;

    for (int i=0; i < PageSize; i++) {
        void *pData = (char *)memBlk + i;
        MemBlock *thisBlk = MemBlock::AddressToMemBlock(pData, PageSize);

        if (thisBlk != memBlk) {
            cout<<"MemBlockTest AddressToMemBlock Failed!"<<endl;
            break;
        }

    }

    cout<<"MemBlockTest AddressToMemBlock OK!"<<endl;

    void *pData;

    cout<<"MemBlockTest Allocate Start..."<<endl;

    for (int i=0; i<40; i++) {
        pData = memBlk->Allocate(100);
        memBlk->Free(pData);
    }

    cout<<"MemBlockTest Allocate OK!"<<endl;
    cout<<"MemBlockTest Unsatisfied Allocate Start..."<<endl;

    pData = memBlk->Allocate(100);

    if (pData == nullptr) {
        cout<<"MemBlockTest Unsatisfied Allocate OK!"<<endl;
    }
    else {
        cout<<"MemBlockTest Unsatisfied Allocate Failed!"<<endl;
    }

    memBlk->Reset();
    pData = memBlk->Allocate(100);

    cout<<"MemBlockTest Reset Test Start..."<<endl;

    if (pData != nullptr) {
        cout<<"MemBlockTest Reset Test OK!"<<endl;
    }
    else {
        cout<<"MemBlockTest Reset Failed!"<<endl;
    }

    memBlk->Free(pData);

	MemBlock::Destroy(memBlk);
	return 0;
}
