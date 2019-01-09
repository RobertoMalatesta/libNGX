namespace ngx {
    namespace Core {
        namespace BasicComponent {
            using namespace ngx::Core::DefaultConfig;
            using namespace ngx::Core::MetaClass;
            using namespace ngx::Core::Types;

/** Abstract components */

#include "Core/BasicComponent/Error.h"
#include "Core/BasicComponent/Reusable.h"
#include "Core/BasicComponent/Collector.h"
#include "Core/BasicComponent/Reference.h"
#include "Core/BasicComponent/SpinLock.h"
#include "Core/BasicComponent/Checksum.h"
#include "Core/BasicComponent/Hash.h"
#include "Core/BasicComponent/Digest.h"
#include "Core/BasicComponent/Time.h"
#include "Core/BasicComponent/Dictionary.h"

/** Pool */
#include "Core/BasicComponent/BasicMemoryBlock.h"
#include "Core/BasicComponent/MemoryBlockAllocator.h"
#include "Core/BasicComponent/Pool.h"
#include "Core/BasicComponent/PoolMemoryBlockCollector.h"

/** ThreadPool */
#include "Core/BasicComponent/ThreadPool.h"

/** SocketTimerHub */
#include "Core/BasicComponent/SocketTimerHub.h"

/** Buffer */
#include "Core/BasicComponent/BufferMemoryBlock.h"
#include "Core/BasicComponent/BufferMemoryBlockCollector.h"
#include "Core/BasicComponent/BufferCursor.h"
#include "Core/BasicComponent/Buffer.h"
#include "Core/BasicComponent/BufferBuilder.h"


/** Event */
#include "Core/BasicComponent/EventEntity.h"
#include "Core/BasicComponent/EventDomain.h"

/** Socket */
#include "Core/BasicComponent/Socket.h"
#include "Core/BasicComponent/SocketEventDomain.h"
#include "Core/BasicComponent/Connection.h"
#include "Core/BasicComponent/Listening.h"

/** Server */
#include "Core/BasicComponent/Server.h"

        }
    }
}