//===- Buffer.h - Buffer to provide block-chained buffer --------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===----------------------------------------------------------------------===//
//
//  This file provide Buffer facility to enable block based Buffer
//
//===----------------------------------------------------------------------===//

#ifndef BUFFER_H
#define BUFFER_H

namespace ngx {
    namespace Core{
        namespace BasicComponent{
            class Cursor;

            class BoundCursor;

            class Connection;

/**
 *  @name Buffer
 *
 *  @brief Block based buffer used to store and write connection data
 * */
            class Buffer : public CanReset, public CanGC {
            protected:

                /** Buffer Memory Block Size */
                size_t BlockSize;

                /** Buffer left - right bound */
                BoundCursor Cursor;

                /** Header of Buffer memory block list */
                BufferMemoryBlock *HeadBlock = nullptr;

                /** BackendCollector to fetch memory block */
                BufferMemoryBlockCollector *Collector = nullptr;

                friend class BufferBuilder;

            public:
                Buffer() = default;

                ~Buffer();

                /** Set the Buffer BoundCursor, used to update left bound */
                Buffer &operator<<(BoundCursor &BC) &;

                /** Get the Buffer BoundCursor, used to iterate the buffer */
                Buffer &operator>>(BoundCursor &BC) &;

                /** Retrieval the BufferMemoryBlock by the memory address */
                inline BufferMemoryBlock *AddressToMemoryBlock(void *Cursor) const {
                    return BufferMemoryBlock::AddressToMemoryBlock(Cursor, BlockSize);
                }

                /** Read Connection data to Buffer and move right bound */
                RuntimeError ReadConnection(Connection *C);

                /** Read raw bytes to Buffer and move right bound */
                RuntimeError ReadBytes(u_char *PointerToData, size_t DataLength);

                /** Write Buffer data to connection and move left bound, return EAGAIN when nonblocking write failed */
                RuntimeError WriteConnection(Connection *C);

                /** Write Buffer data to target buffer */
                RuntimeError WriteBytes(u_char *PointerToData, size_t DataLength);

                /** If the Buffer is empty */
                inline bool IsEmpty() const {
                    return !(Cursor + 1);
                }

                /** Release all memory buffer used */
                virtual void Reset();

                /** Release all blocks before read cursor */
                virtual void GC();
            };

            using Byte = unsigned char;

            class MemoryBuffer {
            protected:
                MemoryBuffer() = default;
                MemoryBuffer(Byte* BufferStart, Byte* BufferEnd);

            private:
                Byte *BufferStart, *BufferEnd;

            public:
                MemoryBuffer(const MemoryBuffer &) = delete;
                MemoryBuffer operator = (const MemoryBuffer &) = delete;

                virtual ~MemoryBuffer() {};

                const Byte *getBufferStart() const { return BufferStart; }

                const Byte *getBufferEnd() const { return BufferEnd; }

//                StringRef getBuffer() const {
//                    return StringRef(BufferStart, getBufferSize());
//                }

                size_t getBufferSize() const { return BufferEnd - BufferStart; }
            };

            class WritableMemoryBuffer : public MemoryBuffer {
            private:
                bool Aligned = false;
            protected:
                WritableMemoryBuffer(Byte *BufferStart, Byte *BufferEnd);

            public:
                using MemoryBuffer::getBufferStart;
                using MemoryBuffer::getBufferEnd;
                using MemoryBuffer::getBufferSize;

                virtual ~WritableMemoryBuffer() {};

                Byte *getBufferStart() const {
                    return const_cast<Byte *>(MemoryBuffer::getBufferStart());
                }

                Byte *getBufferEnd() const {
                    return const_cast<Byte *>(MemoryBuffer::getBufferEnd());
                }

                size_t resize(size_t NewSize);

                static std::unique_ptr<WritableMemoryBuffer> NewBuffer(size_t Size, bool Aligned);
            };

            class StringRef {
//             memory index
            };

            class BufferWriter {
                size_t MaximumBufferSize = -1, Offset = 0;
                WritableMemoryBuffer &Buffer;

            public:
                BufferWriter(WritableMemoryBuffer &Target);
                BufferWriter(WritableMemoryBuffer &Target, size_t MaximumBufferSize, size_t Offset);
                BufferWriter(const BufferWriter &) = delete;
                BufferWriter operator = (const BufferWriter &) = delete;

                int FromByte(const Byte &);
                int FromFile(int FD, uint64_t Size, int64_t Offset = 0);
                int FromSocket(int FD, uint64_t MaximumSize);
            };

        }   //BasicComponent
    }   // Core
}   // ngx

#endif