//===- BufferMemoryCollector.h - Collector for BufferMemoryBlock ---*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file declare a simple collector of BufferMemoryBlock, which aims to
//  reduce the count of malloc(2) procedure
//
//===-------------------------------------------------------------------------===//


/**
 * @name BufferMemoryBlockCollector
 *
 * @brief Collector for BufferMemoryBlock
 *
 * */
class BufferMemoryBlockCollector : public Collector, MetaClass::CanReset {

protected:

    /** Lock to protect the collector */
    SpinLock Lock;

    /** Size of the built BufferMemoryBlock, which can be hot configured */
    size_t BlockSize;

public:
    BufferMemoryBlockCollector(size_t BlockSize = BUFFER_MEMORY_BLOCK_SIZE,
                               uint32_t CollectorSize = DEFAULT_COLLECTOR_SIZE);

    ~BufferMemoryBlockCollector();

    /** Set collector size */
    virtual RuntimeError SetCollectorSize(uint32_t Size);

    /** Get BufferMemoryBlock size */
    size_t GetBlockSize() const {
        return this->BlockSize;
    }

    /** Set BufferMemoryBlock size */
    RuntimeError SetBlockSize(size_t Size);

    /** Get a BufferMemoryBlock item, create if collector empty  */
    virtual BufferMemoryBlock *Get();

    /** Collect a BufferMemoryBlock item, destroy when collector full or BlockSize changed  */
    virtual void Put(BufferMemoryBlock *Item);

    /** Destory all item collected, caller must hold the lock */
    virtual void Reset();
};
