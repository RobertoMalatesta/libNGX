#include <iostream>

#include "Core/MemBlock.h"

using namespace std;
using namespace ngx;
using namespace ngx::Core;

int MemBlockTest() {
	MemBlock *memBlk = MemBlock::New();

    for (int i=0; i < PageSize; i++) {
        void *pData = (char *)memBlk + i;
        MemBlock *thisBlk = MemBlock::AddressToMemBlock(pData, PageSize);

        if (thisBlk != memBlk) {
            cout<<"Test Failed!"<<endl;
            break;
        }

    }

    void *pData;

    for (int i=0; i<41; i++) {
        pData = memBlk->Allocate(100);
    }

	MemBlock::Destroy(memBlk);
	return 0;
}
