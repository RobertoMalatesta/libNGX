//===- BasicMemoryBlock.h - Basic Memory Block Class -----------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===----------------------------------------------------------------------===//
//
// This file defines the basic structure of a Memory Block, and handle Start
// Magic and End position assignment.
//
//===----------------------------------------------------------------------===//

/**
 *  @name BasicMemoryBlock
 *
 *  @brief Basic memory block structure. Implemented following some functions:
 *      1.  fill block header
 *      2.  retrieval MemoryBlock by memory address
 * */
class BasicMemoryBlock : public MetaClass::Achor {

protected:

    /** Total block size */
    size_t BlockSize = 0;
    u_char *Start, *End;
    /** Magic to identify this block */
    void *Magic = nullptr;

    /** Construct a BasicMemoryBlock and fill the header variable specified before */
    BasicMemoryBlock(size_t Size);

    /** Destruct the BasicMemoryBlock. Reset header variable to avoid UAF */
    ~BasicMemoryBlock();

public:

    /** Retrieval a memory block through the Address and Size of the Block */
    static BasicMemoryBlock *AddressToMemoryBlock(void *Address, size_t Size);

    /** Get Memory Block Size */
    size_t GetBlockSize() const { return BlockSize; };
};
