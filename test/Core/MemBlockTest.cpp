#include "Core/Core.h"

using namespace std;
using namespace ngx;
using namespace ngx::Core;

int MemBlockTest() {

    cout << "MemBlock Class Test Start!" << endl;

    MemoryBlockAllocator *memBlk;
    memBlk = MemoryBlockAllocator::Build(4096);

    cout << "MemBlockTest AddressToMemBlock Start..." << endl;

    for (int i = 0; i < PAGE_SIZE; i++) {
        void *pData = (char *) memBlk + i;
        BasicMemoryBlock *thisBlk = BasicMemoryBlock::AddressToMemoryBlock(pData, PAGE_SIZE);

        if (thisBlk != memBlk) {
            cout << "MemBlockTest AddressToMemBlock Failed!" << endl;
            break;
        }

    }

    cout << "MemBlockTest AddressToMemBlock OK!" << endl;

    void *pData;

    cout << "MemBlockTest Allocate Start..." << endl;

    for (int i = 0; i < 40; i++) {
        pData = memBlk->Allocate(100);
        memBlk->Free(pData);
    }

    cout << "MemBlockTest Allocate OK!" << endl;
    cout << "MemBlockTest Unsatisfied Allocate Start..." << endl;

    pData = memBlk->Allocate(100);

    if (pData == nullptr) {
        cout << "MemBlockTest Unsatisfied Allocate OK!" << endl;
    } else {
        cout << "MemBlockTest Unsatisfied Allocate Failed!" << endl;
    }

    memBlk->Reset();
    pData = memBlk->Allocate(100);

    cout << "MemBlockTest Reset Test Start..." << endl;

    if (pData != nullptr) {
        cout << "MemBlockTest Reset Test OK!" << endl;
    } else {
        cout << "MemBlockTest Reset Failed!" << endl;
    }

    memBlk->Free(pData);

    MemoryBlockAllocator::Destroy(memBlk);

    return 0;
}
