#include <stdlib.h>
#include <stdio.h>

#include "Core/MemBlock.h"

namespace ngx::Core {

	MemBlock *MemBlock::New( size_t Size ) {
	
		MemBlock *ret = (MemBlock *) valloc( Size );
		
		if (nullptr == ret) {
			return nullptr;
		}
		
		ret -> PointerToData = ret + sizeof(MemBlock); 
		ret -> TotalSize = Size - sizeof(MemBlock);

		return ret;
	}
	
	void MemBlock::Destroy( MemBlock *memBlk ) {
		if ( nullptr != memBlk && nullptr != memBlk -> PointerToData ) {
			free((void *) memBlk);
		}
	
		return;
	}
}
