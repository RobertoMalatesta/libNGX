namespace ngx::Core {

    class BufferMemoryBlock : public MemoryBlock, public Recyclable {
    protected:
        BufferMemoryBlock *NextBlock = nullptr;

        BufferMemoryBlock(size_t Size);

        ~BufferMemoryBlock();

        friend class Buffer;

    public:
        u_char *Start, *End, *Pos;

        static BufferMemoryBlock *Build(size_t Size);

        static void Destroy(BufferMemoryBlock **PointerToBlock);

        void SetNextBlock(BufferMemoryBlock *Next) { this->NextBlock = Next; }

        BufferMemoryBlock *GetNextBlock() { return NextBlock; }

        virtual void Reset();
    };

}
