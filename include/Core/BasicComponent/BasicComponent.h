namespace ngx {
    namespace Core {
        namespace BasicComponent {
            using namespace ngx::Core::DefaultConfig;
            using namespace ngx::Core::MetaClass;
            using namespace ngx::Core::Types;

#include "Core/BasicComponent/Error.h"
#include "Core/BasicComponent/Recyclable.h"
#include "Core/BasicComponent/Recylcer.h"
#include "Core/BasicComponent/Reference.h"
#include "Core/BasicComponent/BasicMemoryBlock.h"
#include "Core/BasicComponent/MemoryBlockAllocator.h"
#include "Core/BasicComponent/Pool.h"
#include "Core/BasicComponent/SpinLock.h"
#include "Core/BasicComponent/Checksum.h"
#include "Core/BasicComponent/Hash.h"
#include "Core/BasicComponent/Digest.h"
#include "Core/BasicComponent/ThreadPool.h"
#include "Core/BasicComponent/Time.h"
#include "Core/BasicComponent/TimerHub.h"

#include "Core/BasicComponent/BufferMemoryBlock.h"
#include "Core/BasicComponent/BufferMemoryBlockRecycleBin.h"
#include "Core/BasicComponent/BufferCursor.h"
#include "Core/BasicComponent/Dictionary.h"

#include "Core/BasicComponent/EventEntity.h"
#include "Core/BasicComponent/EventDomain.h"
#include "Core/BasicComponent/Socket.h"
#include "Core/BasicComponent/SocketEventDomain.h"
#include "Core/BasicComponent/Connection.h"
#include "Core/BasicComponent/Listening.h"
#include "Core/BasicComponent/Server.h"

#include "Core/BasicComponent/Buffer.h"
#include "Core/BasicComponent/BufferAllocator.h"
#include "Core/BasicComponent/BufferBuilder.h"
        }
    }
}