namespace ngx::Core {

    const size_t DEFAULT_BLOCK_SIZE = 4096;
    const size_t BUFFER_BLOCK_RESERVED = 4;

    class Buffer {
        protected:
            size_t BlockSize = 0;
            MemoryBlock *HeadBlock = nullptr, *CurrentBlock = nullptr;
            MemoryBlock *ReadBlock = nullptr, *WriteBlock = nullptr;
            u_char *ReadPosition= nullptr, *WritePosition = nullptr;
        public:
            Buffer(size_t BlockSize=DEFAULT_BLOCK_SIZE);
            RuntimeError WriteDataToBuffer(u_char *PointerToData, size_t DataLength);
            bool HasByte();
            char *ReadByte();
            void Reset();
    };
}