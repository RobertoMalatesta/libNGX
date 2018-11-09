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

namespace ngx {

    namespace Core {

    class BufferMemoryBlock : public BasicMemoryBlock, public Recyclable, public AlignBuild<BufferMemoryBlock>{
        protected:
            BufferMemoryBlock *NextBlock = nullptr;

            BufferMemoryBlock(size_t Size);

            ~BufferMemoryBlock();

            friend class Buffer;

        public:
            u_char *Start, *End;

            static int Build(BufferMemoryBlock* &Item, size_t Size);

            static int Destroy(BufferMemoryBlock* &Item);

            void SetNextBlock(BufferMemoryBlock *Next) { this->NextBlock = Next; }

            BufferMemoryBlock *GetNextBlock() { return NextBlock; }

            virtual void Reset();
        };

    }
}