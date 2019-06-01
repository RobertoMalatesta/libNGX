#ifndef LIBNGX_ALLOCATOR_H
#define LIBNGX_ALLOCATOR_H

namespace ngx {
    namespace Core {
        namespace ADT {
            class Allocator {
            public:
                virtual void *Allocate(size_t) = 0;

                virtual void Free(void *&) = 0;
            };
        } // ADT
    } // Core
} // ngx

#endif //LIBNGX_ALLOCATOR_H
