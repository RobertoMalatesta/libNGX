#include "Core/Core.h"

using namespace ngx::Core;

uint32_t Reference::GetReference() {
    return Counter.fetch_add(1UL);
}

uint32_t Reference::PutReference() {
    return Counter.fetch_sub(1UL);
}

uint32_t Reference::ReferenceCount() {
    return Counter;
}

void Reference::ClearReference() {
    Counter.exchange(0);
}