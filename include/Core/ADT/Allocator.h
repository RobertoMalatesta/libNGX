#ifndef LIBNGX_ALLOCATOR_H
#define LIBNGX_ALLOCATOR_H
class Allocator {
public:
    virtual void *Allocate(size_t) = 0;

    virtual void Free(void *&) = 0;
};

#endif //LIBNGX_ALLOCATOR_H
