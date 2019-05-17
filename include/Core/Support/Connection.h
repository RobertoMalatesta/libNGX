//===--------------- Connection.h - represent a Connection ---------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file declare the Connection basic class
//
//===-------------------------------------------------------------------------===//

/**
 *  @name Connection
 *
 *  @brief Peer connection
 **/
class Connection : public EventSubscriber{
protected:
    int FD;
    Timer TimerNode;
    Address_t Address;
    virtual RuntimeError OnEvent(Event_t E) { return {0}; };

public:
    spin_lock Lock;

    Connection(): FD(-1), Address() {}

    Connection(int FD, Address_t &Addr): FD(FD), Address(Addr) {};

    inline int getFD() const { return FD; };

    SocketError setNoBlock(unsigned On);

    SocketError setNoDelay(unsigned On);

    SocketError close();

    virtual SocketError connect(Address_t &Addr) = 0;

    static inline Connection *timerToConnection(Timer *T) {
        if (T == nullptr) {
            return nullptr;
        }
        return reinterpret_cast<Connection *>(((uintptr_t)T - (uintptr_t)(&((Connection *) 0)->TimerNode)));
    }
};

