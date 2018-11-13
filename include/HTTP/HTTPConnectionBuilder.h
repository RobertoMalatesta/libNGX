//===- HTTPConnectionBuilder.h - Get and config HTTPConnection -----*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file manage HTTPConnection`s build and configuration
//
//===-------------------------------------------------------------------------===//

namespace ngx {
    namespace HTTP {
        class HTTPConnectionBuilder {
        protected:
            SpinLock Lock;
            BufferBuilder BB;
            HTTPConnectionRecycler BackendRecycler;
            uint32_t TCPNoDelay;
            uint32_t TCPNoPush;
            uint32_t ReadBufferSize = 8 * 1024 * 1024;
            uint32_t WriteBufferSize = 8 * 1024 * 1024;
            uint32_t ReadTimeout = 1 * 60;
            uint32_t WriteTimeout = 1 * 60;

        public:
            HTTPConnectionBuilder(size_t BufferBlockSize, uint64_t BufferRecyclerSize, uint64_t ConnectionRecyclerSize);

            inline HTTPError SetTCPNoDelay(bool Open) {
                TCPNoDelay = (Open) ? 1 : 0;
                return {0};
            }

            inline HTTPError SetTCPNoPush(bool Open) {
                TCPNoPush = (Open) ? 1 : 0;
                return {0};
            };

            inline HTTPError SetReadBufferSize(uint32_t Size) {
                ReadBufferSize = Size;
                return {0};
            };

            HTTPConnection *Get(int SocketFD, SocketAddress *SocketAddress, HTTPServer *Server, Listening *Listening,
                                SocketEventDomain *EventDomain);

            void Put(HTTPConnection *C);
        };
    }
}