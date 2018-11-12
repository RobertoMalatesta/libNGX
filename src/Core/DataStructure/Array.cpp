#include "Core/Core.h"

using namespace ngx::Core::DataStructure;

static inline void * Allocate(MemAllocator *Allocator, size_t Size) {

    if (Allocator != nullptr) {
        return Allocator->Allocate(Size);
    } else {
        return malloc(Size);
    }
}

static inline void Free(MemAllocator *Allocator, void *&Pointer) {

    if (Allocator != nullptr) {
        return Allocator->Free(Pointer);
    } else {
        free(Pointer);
        Pointer = nullptr;
    }
}

Array::Array(MemAllocator *Allocator, size_t Size, uint Count) {

    this->Allocator = Allocator;
    this->Size = Size;
    this->NAlloc = Count;

    PointerToData = (u_char *)Allocate(this->Allocator, Count *Size);
}

Array::~Array() {
    Free(Allocator, (void * &)PointerToData);
}

void *Array::Push() {

    if (ElementCount == NAlloc) {

        void *NewDataPointer = Allocate(Allocator, 2 * (Size *NAlloc));

        if (NewDataPointer == nullptr) {
            return nullptr;
        }

        memcpy(NewDataPointer, PointerToData, (Size * NAlloc));
        Free(Allocator, (void * &) PointerToData);
        PointerToData = (u_char *)NewDataPointer;
        NAlloc = 2 * NAlloc;
    }

    u_char *Ret = (u_char *) PointerToData + Size * ElementCount;
    ElementCount += 1;

    return Ret;
}

void *Array::PushN(uint N) {

    if (ElementCount + N > NAlloc) {

        void *NewDataPointer;

        uint32_t NewAlloc = 2 * (N >= NAlloc ? N : NAlloc);

        NewDataPointer = Allocate(Allocator, NewAlloc *Size);

        if (NewDataPointer == nullptr) {
            return nullptr;
        }

        memcpy(NewDataPointer, PointerToData, (Size * ElementCount));

        Free(Allocator, (void * & )PointerToData);
        PointerToData = (u_char *)NewDataPointer;
        NAlloc = NewAlloc;
    }

    u_char *Ret = (u_char *) PointerToData + Size * ElementCount;
    ElementCount += N;

    return Ret;
}


