#include "Core/Support/Reusable.h"
#include "Core/Support/Hash.h"
#include "Core/Support/Checksum.h"
#include "Core/Support/Digest.h"
#include "Core/Support/Time.h"
#include "Core/Support/Error.h"
#include "Core/Support/spin_lock.h"
#include "Core/Support/BasicMemoryBlock.h"
#include "Core/Support/BufferMemoryBlock.h"
#include "Core/Support/Pool.h"
#include "Core/Support/ThreadPool.h"
#include "Core/Support/TimerTree.h"
#include "Core/Support/Cursor.h"
#include "Core/Support/BoundCursor.h"

#ifndef LIBNGX_CORE_SUPPORT_REMAIN1
#define LIBNGX_CORE_SUPPORT_REMAIN1
namespace ngx {
    namespace Core {
        namespace Support {
            using Byte = unsigned char;
#include "Core/Support/Collector.h"
#include "Core/Support/Dictionary.h"
#include "Core/Support/PoolMemoryBlockCollector.h"
/** Buffer */
#include "Core/Support/BufferMemoryBlockCollector.h"
#include "Core/Support/BufferBuilder.h"
        }   // Support
    }   // Core
}   // ngx
#endif

#include "Core/Support/EventDomain.h"
#include "Core/Support/Socket.h"
#include "Core/Support/Connection.h"
#include "Core/Support/Listen.h"
#include "Core/Support/TCPConnection.h"
#include "Core/Support/TCPListen.h"

#ifndef LIBNGX_CORE_SUPPORT_REMAIN2
#define LIBNGX_CORE_SUPPORT_REMAIN2
namespace ngx {
namespace Core {
namespace Support {
/** Server */
#include "Core/Support/Server.h"
} // Support
} // Core
} // ngx
#endif

#include "Core/Support/Buffer.h"
#include "Core/Support/MemoryBuffer.h"
