#include "Core/Core.h"

using namespace ngx::Core;

Buffer BufferBuilder::BuildBuffer() {
    return Buffer(&Recycler, BlockSize);
}
