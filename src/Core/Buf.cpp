#include "Core/Core.h"

namespace ngx::Core {

    Buf::Buf(Pool *Allocator, size_t Size) {

        this -> Allocator = nullptr;
        Start = End = Pos = Last = nullptr;
        Shadow = nullptr;

        temporary = memory = mmap = recycled = 0;
        in_file = flush = sync = last_buf = last_in_chain = 0;
        last_shadow = temp_file = 0;

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
            Allocator -> Free(Start);
            Start = End = Pos = Last = nullptr;
            Allocator = nullptr;
        }
    }
}