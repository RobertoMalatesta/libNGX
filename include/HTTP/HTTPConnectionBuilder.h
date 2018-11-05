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
        class HTTPConnectionBuidler {
        protected:
            HTTPConnectionRecycler BackendRecycler;
            unsigned TCPNoDelay: 1;
            unsigned TCPNoPush: 1;
            uint32_t ReadBufferSize = 8 * 1024 * 1024;
            uint32_t WriteBufferSize = 8 * 1024 * 1024;
            uint32_t ReadTimeout = 1 * 60;
            uint32_t WriteTimeout = 1 * 60;

        public:
            HTTPError SetTCPNoDelay(bool Open);
            HTTPError SetTCPNoPush(bool Open);
            HTTPError SetReadBufferSize();
            HTTPConnection *Get();
            void Put(HTTPConnection *C);
        };
    }
}