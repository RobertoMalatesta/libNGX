//===- Array.h - Array to hold contiguous element   -------------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===----------------------------------------------------------------------===//
//
//  This file provide Array facility to enable alloc contiguous elements
//
//===----------------------------------------------------------------------===//

template<typename T>
class Array {
private:
    Allocator *BackendAllocator = nullptr;
    u_char *PointerToData = nullptr;
    uint32_t AllocateCount = 0;
    uint32_t ElementCount = 0;

public:
    Array(Allocator *Allocator) {

        // specify memory allocator, use malloc() if nullptr
        this->BackendAllocator = Allocator;
    }

    ~Array() {

        // free all memory used
        Reset();
    }

    T *operator[](uint32_t Index) const {

        // not init yet?
        if (PointerToData == nullptr || Index >= ElementCount) {
            return nullptr;
        }

        return (T *) (PointerToData + (Index * sizeof(T)));
    }

    uint32_t Size() const {

        return ElementCount;
    }

    T *Push() {

        return PushN(1);
    }

    T *PushN(uint32_t N) {

        // need init ?
        if (AllocateCount == 0 || PointerToData == nullptr) {
            if (BackendAllocator != nullptr) {
                PointerToData = (u_char *) BackendAllocator->Allocate(ARRAY_DEFAULT_ELEMENT_COUNT * sizeof(T));
            } else {
                PointerToData = (u_char *) malloc(ARRAY_DEFAULT_ELEMENT_COUNT * sizeof(T));
            }

            if (PointerToData != nullptr) {
                AllocateCount = ARRAY_DEFAULT_ELEMENT_COUNT;
            } else {
                return nullptr;
            }
        }

        // need reallocate?
        if (ElementCount + N > AllocateCount) {

            void *NewDataPointer;

            uint32_t NewAlloc = 2 * (N >= AllocateCount ? N : AllocateCount);

            if (BackendAllocator != nullptr) {
                NewDataPointer = BackendAllocator->Allocate(NewAlloc * sizeof(T));
            } else {
                NewDataPointer = malloc(NewAlloc * sizeof(T));
            }

            if (NewDataPointer == nullptr) {
                return nullptr;
            }

            memcpy(NewDataPointer, PointerToData, (sizeof(T) * ElementCount));

            if (BackendAllocator != nullptr) {
                BackendAllocator->Free((void *&) PointerToData);
            } else {
                free(PointerToData);
            }

            Reset();
            PointerToData = (u_char *) NewDataPointer;
            AllocateCount = NewAlloc;
        }

        // allocate Array item
        u_char *Ret = PointerToData + sizeof(T) * ElementCount;
        ElementCount += N;

        return (T *) Ret;
    }

    virtual void Reset() {

        // free all memory and reset state
        if (BackendAllocator != nullptr) {
            BackendAllocator->Free((void *&) PointerToData);
        } else {
            free(PointerToData);
        }

        PointerToData = nullptr, AllocateCount = ElementCount = 0;
    }
};