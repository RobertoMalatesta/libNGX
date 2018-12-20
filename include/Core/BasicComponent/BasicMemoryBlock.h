class BasicMemoryBlock {
protected:
    size_t TotalSize = 0;
    u_char *Start, *End;
    void *Magic = nullptr;

    BasicMemoryBlock(size_t Size);

    ~BasicMemoryBlock();

public:

    static BasicMemoryBlock *AddressToMemoryBlock(void *Address, size_t Size);

    bool IsInBlock(void *Address);
};
