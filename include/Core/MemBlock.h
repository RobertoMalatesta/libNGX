#include "Core/Core.h"

namespace ngx::Core {
	static size_t PageSize = 4096;
	
	class MemBlock {	
	    private:
			void *PointerToData;
			size_t TotalSize;
			~MemBlock() {};
			MemBlock() {};
	    public: 
			static MemBlock *New( size_t Size = PageSize );
			static void Destroy(MemBlock *memBlk);
	};
}


