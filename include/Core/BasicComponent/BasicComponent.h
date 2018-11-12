namespace ngx {
    namespace Core {
        namespace BasicComponent {
            using namespace ngx::Core::DefaultConfig;
            using namespace ngx::Core::MetaClass;
            using namespace ngx::Core::DataStructure;

#include "Core/BasicComponent/Error.h"
#include "Core/BasicComponent/Recyclable.h"
#include "Core/BasicComponent/Recylcer.h"
#include "Core/BasicComponent/Reference.h"
#include "Core/BasicComponent/BasicMemoryBlock.h"
#include "Core/BasicComponent/MemoryBlockAllocator.h"
#include "Core/BasicComponent/Pool.h"
#include "Core/BasicComponent/Lock.h"
#include "Core/BasicComponent/Checksum.h"
#include "Core/BasicComponent/Hash.h"
#include "Core/BasicComponent/Digest.h"
#include "Core/BasicComponent/ThreadPool.h"
#include "Core/BasicComponent/Time.h"
#include "Core/BasicComponent/TimerTree.h"

#include "Core/BasicComponent/EventEntity.h"
#include "Core/BasicComponent/Socket.h"
#include "Core/BasicComponent/EventDomain.h"
#include "Core/BasicComponent/SocketEventDomain.h"
#include "Core/BasicComponent/Connection.h"
#include "Core/BasicComponent/Listening.h"

#include "Core/BasicComponent/BufferMemoryBlock.h"
#include "Core/BasicComponent/BufferMemoryBlockRecycler.h"
#include "Core/BasicComponent/Buffer.h"
#include "Core/BasicComponent/BufferAllocator.h"
#include "Core/BasicComponent/BufferBuilder.h"

        }
    }
}