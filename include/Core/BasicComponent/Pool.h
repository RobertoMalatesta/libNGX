//===------------------ Pool.h - Implement memory pool ------------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  define and implement pool
//
//===-------------------------------------------------------------------------===//

class Pool {

private:
    uint32_t AllocateRound = 0;

protected:
    const size_t BLOCK_SIZE = POOL_MEMORY_BLOCK_SIZE;

    class PoolMemoryBlock {
    protected:
        int32_t RefCount;
        u_char *Start, *End, *Pos;
        PoolMemoryBlock *Next;
        PoolMemoryBlock():RefCount(0), Start(nullptr), End(nullptr), Pos(nullptr), Next(nullptr){};

    public:
        ~PoolMemoryBlock() = default;

        Byte *allocate(size_t Size);

        void free(Byte *&Pointer);

        /** Get start address from address of end */
        inline bool IsInBlock(void *Address) const {
            return Address >= Start && Address < End;
        }

        inline PoolMemoryBlock *setNextBlock(PoolMemoryBlock *N) {
            PoolMemoryBlock *OldN = this->Next;
            this->Next = N;
            return OldN;
        }

        inline PoolMemoryBlock *getNextBlock() { return Next; }

        inline bool IsFreeBlock() {
            return RefCount == 0;
        }

        static PoolMemoryBlock *newBlock(size_t Size);
    };

    PoolMemoryBlock *HeadBlock, *CurrentBlock;

public:
    Pool();
    Pool(Pool &Copy) = delete;
    Pool operator=(Pool &) = delete;

    ~Pool();

    void *allocate(size_t Size);

    void free(void *&pointer);

    void collect();

    virtual void reset();
};

