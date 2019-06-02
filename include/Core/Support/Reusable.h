#include "Core/ADT/qnode.h"

#ifndef LIBNGX_CORE_SUPPORT_REUSE
#define LIBNGX_CORE_SUPPORT_REUSE
namespace ngx {
namespace Core {
namespace Support {

using namespace ADT;

class Reusable {
public:
    qnode ReuseItem;

    Reusable() : ReuseItem() {};

    virtual void Reset() {};
};

} // Support
} // Core
} // ngx
#endif
