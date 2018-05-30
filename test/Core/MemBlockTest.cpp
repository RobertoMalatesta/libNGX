#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "Core/MemBlock.h"

using namespace std;
using namespace ngx;
using namespace ngx::Core;

int MemBlockTest() {
	MemBlock *memBlk = MemBlock::New();
	MemBlock::Destroy(memBlk);
	return 0;
}
