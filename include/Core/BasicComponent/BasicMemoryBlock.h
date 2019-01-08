//===- BasicMemoryBlock.h - Basic Memory Block Class -----------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===----------------------------------------------------------------------===//
//
// This file define the basic structure of a Memory Block, and handle Start
// Magic and End position assignment.
//
//===----------------------------------------------------------------------===//

/**
 *  @name Basic memory block
 *
 *  @brief Basic memory block structure.
 * */
class BasicMemoryBlock {
protected:
    size_t TotalSize = 0;
    u_char *Start, *End;
    void *Magic = nullptr;

    BasicMemoryBlock(size_t Size);

    ~BasicMemoryBlock();

public:

    /** Retrieval a memory block through the Address and Size of the Block */
    static BasicMemoryBlock *AddressToMemoryBlock(void *Address, size_t Size);

    /** Does this address belongs to this Basic Memory Block? */
    bool IsInBlock(void *Address);
};
