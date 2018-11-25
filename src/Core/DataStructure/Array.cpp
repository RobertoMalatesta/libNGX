#include "Core/Core.h"

using namespace ngx::Core::DataStructure;

static inline void *Allocate(MemAllocator *Allocator, size_t Size) {

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

template <typename T>
Array<T>::Array(MemAllocator *Allocator, uint32_t Count) {

    this->Allocator = Allocator;
    this->NAlloc = Count;

    PointerToData = (u_char *) Allocate(this->Allocator, Count * sizeof(T));
}

template <typename T>
Array<T>::~Array() {
    Free(Allocator, (void *&) PointerToData);
}

template <typename T>
T *Array<T>::Push() {

    if (ElementCount == NAlloc) {

        void *NewDataPointer = Allocate(Allocator, 2 * (sizeof(T) * NAlloc));

        if (NewDataPointer == nullptr) {
            return nullptr;
        }

        memcpy(NewDataPointer, PointerToData, (sizeof(T) * NAlloc));
        Free(Allocator, (void *&) PointerToData);
        PointerToData = (u_char *) NewDataPointer;
        NAlloc = 2 * NAlloc;
    }

    u_char *Ret = (u_char *) PointerToData + sizeof(T) * ElementCount;
    ElementCount += 1;

    return Ret;
}

template <typename T>
T *Array<T>::PushN(uint32_t N) {

    if (ElementCount + N > NAlloc) {

        void *NewDataPointer;

        uint32_t NewAlloc = 2 * (N >= NAlloc ? N : NAlloc);

        NewDataPointer = Allocate(Allocator, NewAlloc * sizeof(T));

        if (NewDataPointer == nullptr) {
            return nullptr;
        }

        memcpy(NewDataPointer, PointerToData, (sizeof(T) * ElementCount));

        Free(Allocator, (void *&) PointerToData);
        PointerToData = (u_char *) NewDataPointer;
        NAlloc = NewAlloc;
    }

    u_char *Ret = (u_char *) PointerToData + sizeof(T) * ElementCount;
    ElementCount += N;

    return Ret;
}


