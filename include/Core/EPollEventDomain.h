//===- EPollEventDomain.h - Implement epoll based event domain -----*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file implement epoll based socket file EventDomain.
//
//===-------------------------------------------------------------------------===//

namespace ngx {
    namespace Core {

        namespace DefaultConfig {

            const int EPOLL_EVENT_BATCH_SIZE = 512;
            const int EPOLL_EVENT_WAIT_TIME = 1;    // ms
            const int EPOLL_EVENT_MAX_CONNECTION = 32768;
        }

//===-------------------------------------------------------------------------===//
//         EPollArguments
//
//         UserArgument[0] = temp arg0
//         UserArgument[1] = temp arg1
//         UserArgument[2] = temp arg2
//         UserArgument[3] = Server
//         UserArgument[4] = EventDomain
//         UserArgument[5] = Listening
//         UserArgument[6] = Socket
//         UserArgument[7] = EventType
//===-------------------------------------------------------------------------===//

        class EPollEventDomain : public SocketEventDomain {
        protected:
            int EPollFD = -1;
            std::atomic_flag Waiting = ATOMIC_FLAG_INIT;
        public:

            EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize);

            ~EPollEventDomain();

            virtual RuntimeError EventDomainProcess(Server *S);

            virtual EventError AttachSocket(Socket *S, SocketEventType Type);

            virtual EventError DetachSocket(Socket *S, SocketEventType Type);
        };
    }
}