

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
template <typename T>
class Array: public CanReset{
private:
    MemAllocator *Allocator = nullptr;
    u_char *PointerToData = nullptr;
    uint32_t AllocateCount = 0;
    uint32_t ElementCount = 0;

public:
    Array(MemAllocator *Allocator, uint32_t Count = ARRAY_DEFAULT_ELEMENT_COUNT) {
        this->Allocator = Allocator;

        if (Allocator != nullptr) {
            PointerToData = (u_char *)Allocator->Allocate(Count * sizeof(T));
        } else {
            PointerToData = (u_char *)malloc(Count * sizeof(T));
        }
    }

    ~Array() {
        Reset();
    }

    void *operator[](uint32_t Index) {

        if (PointerToData == nullptr || Index >= ElementCount) {
            return nullptr;
        }

        return PointerToData + (Index * sizeof(T));

    }

    T *Push() {
        return PushN(1);
    }

    T *PushN(uint32_t N) {

        if (AllocateCount == 0 || PointerToData == nullptr) {
            if (Allocator != nullptr) {
                PointerToData = (u_char *)Allocator->Allocate(ARRAY_DEFAULT_ELEMENT_COUNT * sizeof(T));
            } else {
                PointerToData = (u_char *)malloc(ARRAY_DEFAULT_ELEMENT_COUNT * sizeof(T));
            }

            if (PointerToData != nullptr) {
                AllocateCount = ARRAY_DEFAULT_ELEMENT_COUNT;
            } else {
                return nullptr;
            }
        }

        if (ElementCount + N > AllocateCount) {

            void *NewDataPointer;

            uint32_t NewAlloc = 2 * (N >= AllocateCount ? N : AllocateCount);

            if (Allocator != nullptr) {
                NewDataPointer = Allocator->Allocate(NewAlloc * sizeof(T));
            } else {
                NewDataPointer = malloc(NewAlloc * sizeof(T));
            }

            if (NewDataPointer == nullptr) {
                return nullptr;
            }

            memcpy(NewDataPointer, PointerToData, (sizeof(T) * ElementCount));

            if (Allocator != nullptr) {
                Allocator->Free((void *&)PointerToData);
            } else {
                free(PointerToData);
            }

            Reset();
            PointerToData = (u_char *) NewDataPointer;
            AllocateCount = NewAlloc;
        }

        u_char *Ret = PointerToData + sizeof(T) * ElementCount;
        ElementCount += N;

        return (T *)Ret;
    }

    virtual void Reset() {
        if (Allocator != nullptr) {
            Allocator->Free((void *&)PointerToData);
        } else {
            free(PointerToData);
        }
        PointerToData = nullptr, AllocateCount = ElementCount = 0;
    }
};