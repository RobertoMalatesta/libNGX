class Allocator {
public:
    virtual void *Allocate(size_t Size) = 0;

    virtual void Free(void *&Pointer) = 0;
};
