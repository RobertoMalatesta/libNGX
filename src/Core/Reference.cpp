#include "Core/Core.h"

using namespace ngx::Core;

uint32_t Reference::IncRef() {
    return Counter.fetch_add(1UL);
}

uint32_t Reference::DecRef() {
    return Counter.fetch_sub(1UL);
}

uint32_t Reference::RefCount() {
    return Counter;
}

void Reference::ClearRef() {
    Counter.exchange(0);
}