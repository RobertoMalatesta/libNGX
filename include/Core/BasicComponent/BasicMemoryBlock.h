class BasicMemoryBlock: public Reference{
protected:
    size_t TotalSize = 0;
    size_t FreeSize = 0;
    void *PointerToData = nullptr, *PointerToHead = nullptr;
    void *Magic = nullptr;

    BasicMemoryBlock(size_t Size);
    ~BasicMemoryBlock();
public:

    static BasicMemoryBlock *AddressToMemoryBlock(void *Address, size_t Size);

    bool IsInBlock(void *Address);

    inline bool IsFreeBlock() {
        return RefCount() == 0;
    }
};
