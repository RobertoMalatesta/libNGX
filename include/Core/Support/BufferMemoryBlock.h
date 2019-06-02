//===- BufferMemoryBlock.h - Hold a aligned memory for buffer ------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file declare a BufferMemoryBlock to hold page aligned memory block to
//  construct buffers.
//
//===-------------------------------------------------------------------------===//
#include <new>

#include "Core/ADT/qnode.h"
#include "Core/Support/Reusable.h"
#include "Core/Support/BasicMemoryBlock.h"

#ifndef LIBNGX_CORE_SUPPORT_BUFFERMMB
#define LIBNGX_CORE_SUPPORT_BUFFERMMB

namespace ngx {
namespace Core {
namespace Support {

using namespace ngx::Core::ADT;

class BufferMemoryBlock
        : public BasicMemoryBlock,
          public Reusable {
protected:
    BufferMemoryBlock *NextBlock = nullptr;

    friend class Buffer;

    friend class BoundCursor;

public:

    BufferMemoryBlock(size_t Size);

    ~BufferMemoryBlock() = default;

    virtual void Reset();

    void SetNextBlock(BufferMemoryBlock *Next) { this->NextBlock = Next; }

    BufferMemoryBlock *GetNextBlock() { return NextBlock; }

    static BufferMemoryBlock *AddressToMemoryBlock(void *Address, size_t Size);

    static inline BufferMemoryBlock *FromCollectorqnode(qnode *Q) {

        if (Q == nullptr) {
            return nullptr;
        }
        return (BufferMemoryBlock *) ((uintptr_t) Q - (uintptr_t)(&((BufferMemoryBlock *) 0)->ReuseItem));
    }

    static inline BufferMemoryBlock *Build(size_t Size) {

        void *TempPointer = nullptr;

        int Code = posix_memalign(&TempPointer, Size, Size);

        if (Code != 0 || TempPointer == nullptr) {
            return nullptr;
        }

        return new(TempPointer) BufferMemoryBlock(Size);
    };

    static inline int Destroy(BufferMemoryBlock *&Item) {

        if (nullptr == Item) {
            return 0;
        }

        free((void *) Item);

        Item = nullptr;
        return 0;
    };
};

} // Support
} // Core
} // ngx

#endif