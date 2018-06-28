#include "Core/Core.h"

namespace ngx::Core {

    Buf::Buf(MemAllocator *Allocator, size_t Size) {

        if (nullptr == Allocator) {
            return;
        }

        this -> Allocator = Allocator;
        this -> Start = (u_char *)this -> Allocator ->Allocate(Size);

        if (nullptr == Start) {
            return;
        }

        Last = Pos = Start;
        End = Last + Size;
        temporary = 1;
    }

    Buf::~Buf() {
        if (nullptr != Allocator && nullptr != Start) {
            Allocator -> Free((void **)&Start);
            Start = End = Pos = Last = nullptr;
            Allocator = nullptr;
        }
    }
}