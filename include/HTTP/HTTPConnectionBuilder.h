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

class HTTPConnectionBuilder {
protected:
    spin_lock Lock;

    BufferBuilder BB;
    Pool BackendAllocator;

    bool NonBlock;
    uint32_t TCPNoDelay;
    uint32_t TCPNoPush;
    uint32_t ReadBufferSize = 8 * 1024 * 1024;
    uint32_t WriteBufferSize = 8 * 1024 * 1024;
    uint32_t ReadTimeout = 1 * 60;
    uint32_t WriteTimeout = 1 * 60;

public:
    HTTPConnectionBuilder(size_t BufferBlockSize, uint32_t BufferCollectorSize);

    ~HTTPConnectionBuilder() = default;

    inline HTTPError SetTCPNoDelay(bool On) {

        std::lock_guard<spin_lock> g(Lock);
        TCPNoDelay = (On) ? 1 : 0;
        return {0};
    }

    inline HTTPError SetTCPNoPush(bool On) {

        std::lock_guard<spin_lock> g(Lock);
        TCPNoPush = (On) ? 1 : 0;
        return {0};
    };

    inline HTTPError SetReadBufferSize(uint32_t Size) {

        std::lock_guard<spin_lock> g(Lock);
        ReadBufferSize = Size;
        return {0};
    };

    int Get(HTTPConnection *&C,
            int SocketFD,
            Address_t &Address,
            HTTPServer *Server,
            SocketEventDomain *EventDomain);

    int Put(HTTPConnection *&C);

    void Reset();

    int Build(HTTPConnection *&C) { return 0; };

    int Destroy(HTTPConnection *&C) { return 0; };
};
