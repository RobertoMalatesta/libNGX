#ifndef MEMORY_BUFFER_H
#define MEMORY_BUFFER_H

namespace ngx {
    namespace Core {
        namespace BasicComponent {


            class StringRef {
            private:
                Byte *Data = nullptr;

                size_t Length = 0;
            public:

                using iterator = const Byte *;
                using const_iterator = const Byte *;
                using size_type = const char *;

                StringRef() = default;

                StringRef(std::nullptr_t) = delete;

                __attribute__ ((always_inline))
                StringRef(Byte *Data, size_t Length): Data(Data), Length(Length) {};

                __attribute__ ((always_inline))
                iterator begin() const { return Data; }

                __attribute__ ((always_inline))
                iterator end() const { return Data + Length; }

                __attribute__ ((always_inline))
                const char * char_begin() const { return reinterpret_cast<const char *>(begin()); }

                __attribute__ ((always_inline))
                const char * char_end() const { return reinterpret_cast<const char *>(end()); }

                __attribute__ ((always_inline))
                const Byte * data() const { return Data; }

                __attribute__ ((always_inline))
                const size_t size() const { return Length; }
            };

            class MemoryBuffer {
            protected:
                Byte *BufferStart, *BufferEnd;
                MemoryBuffer(Byte *BufferStart, Byte *BufferEnd);

            public:
                MemoryBuffer(const MemoryBuffer &) = delete;

                MemoryBuffer operator=(const MemoryBuffer &) = delete;

                virtual ~MemoryBuffer();

                /** return const start pointer */
                __attribute__ ((always_inline))
                const Byte *getBufferStart() const { return BufferStart; }

                /** return const end pointer */
                __attribute__ ((always_inline))
                const Byte *getBufferEnd() const { return BufferEnd; }

                __attribute__ ((always_inline))
                StringRef getBuffer() const {
                    return {BufferStart, getBufferSize()};
                }

                __attribute__ ((always_inline))
                size_t getBufferSize() const { return BufferEnd - BufferStart; }
            };

            class WritableMemoryBuffer : public MemoryBuffer {
            private:
                bool Aligned;
            protected:
                WritableMemoryBuffer(Byte *BufferStart, Byte *BufferEnd, bool Aligned = false);

            public:
                using MemoryBuffer::getBufferStart;
                using MemoryBuffer::getBufferEnd;
                using MemoryBuffer::getBufferSize;

                virtual ~WritableMemoryBuffer() = default;

                /** return writable start pointer */
                __attribute__ ((always_inline))
                Byte *getBufferStart() const {
                    return const_cast<Byte *>(MemoryBuffer::getBufferStart());
                }

                /** return writable end pointer */
                __attribute__ ((always_inline))
                Byte *getBufferEnd() const {
                    return const_cast<Byte *>(MemoryBuffer::getBufferEnd());
                }

                /** resize the buffer to NewSize, will drop data when NewSize is smaller*/
                size_t resize(size_t NewSize);

                static std::unique_ptr <WritableMemoryBuffer> NewBuffer(size_t Size, bool Aligned);
            };

            class BufferWriter {
                size_t MaximumBufferSize = -1, Cursor = 0;
                WritableMemoryBuffer &Buffer;

                size_t CheckBufferSize(size_t WriteSize);

            public:
                explicit BufferWriter(WritableMemoryBuffer &Target) ;

                BufferWriter(WritableMemoryBuffer &Target, size_t MaximumBufferSize, size_t Offset);

                BufferWriter(const BufferWriter &) = delete;

                BufferWriter operator=(const BufferWriter &) = delete;

                /** set the maximum allowed buffer size */
                size_t setMaximumBufferSize(size_t NewSize) {
                    size_t OldSize = MaximumBufferSize;
                    MaximumBufferSize = NewSize;
                    return OldSize;
                }

                int fromByte(Byte B);

                int fromString(const char *Str, size_t Size);

                int fromFile(int FD, int64_t Offset = 0, size_t Size = -1);

                int fromSocket(int FD, uint64_t MaximumSize);

                int toFile(int FD, int64_t Offset = 0, size_t Size = -1);

                int toSocket(int FD, int64_t Offset = 0, size_t Size = -1);
            };
        }   //BasicComponent
    }   // Core
}   // ngx

#endif //MEMORY_BUFFER_H
