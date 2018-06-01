#include "Core/Core.h"

namespace ngx::Core {

    Array::Array(Pool *Allocator, size_t Size, int Count) {

        this -> Allocator = Allocator;
        ElementCount = 0;
        PointerToData = nullptr;
        this -> Size = 0;
        NAlloc = 0;

        PointerToData = this -> Allocator ->Allocate( Count * Size);

        if (nullptr == PointerToData) {
            return;
        }

        this -> Size = Size;
        NAlloc = Count;
    }

    Array::~Array() {

        if (Allocator != nullptr && PointerToData != nullptr) {
            Allocator -> Free(PointerToData);
            PointerToData = nullptr;
        }

        Size = 0;
        NAlloc = 0;
        ElementCount = 0;
        Allocator = nullptr;
    }

    void *Array::Push() {

        if (ElementCount == NAlloc) {

            void *NewDataPointer;

            NewDataPointer = Allocator ->Allocate( 2 * (Size * NAlloc));

            if (NewDataPointer == nullptr) {
                return nullptr;
            }

            memcpy(NewDataPointer, PointerToData, (Size * NAlloc));

            Allocator -> Free(PointerToData);
            PointerToData = NewDataPointer;
            NAlloc = 2 * NAlloc;
        }

        u_char *Ret = (u_char *)PointerToData + Size * ElementCount;
        ElementCount += 1;

        return  Ret;
    }

    void *Array::PushN(uint N) {

        if (ElementCount + N > NAlloc) {

            void *NewDataPointer;

            uint  NewAlloc = 2 * ( N >= NAlloc ? N : NAlloc);

            NewDataPointer = Allocator ->Allocate(NewAlloc * Size);

            if (NewDataPointer == nullptr) {
                return nullptr;
            }

            memcpy(NewDataPointer, PointerToData, (Size * ElementCount));

            Allocator -> Free(PointerToData);
            PointerToData = NewDataPointer;
            NAlloc = NewAlloc;
        }

        u_char *Ret = (u_char *)PointerToData + Size * ElementCount;
        ElementCount += N;

        return  Ret;
    }
}