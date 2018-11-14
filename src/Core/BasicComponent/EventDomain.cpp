#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

EventDomain::EventDomain(size_t PoolSize, int ThreadCount) : Lock(), Allocator(PoolSize), TPool(ThreadCount),
                                                             Timers(&Allocator) {}

