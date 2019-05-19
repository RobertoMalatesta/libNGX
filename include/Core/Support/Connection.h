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
namespace ngx {
    namespace Core {
        namespace Support {
/**
 *  @name Connection
 *
 *  @brief Peer connection
 **/
            class Connection : public EventSubscriber {
            protected:
                int FD;
                Address_t Address;

            public:

                Connection() : FD(-1), Address() {}

                Connection(int FD, Address_t &Addr) : FD(FD), Address(Addr) {};

                inline int getFD() const { return FD; };

                SocketError setNoBlock(unsigned On);

                SocketError setNoDelay(unsigned On);

                SocketError close();

                virtual SocketError connect(Address_t &Addr) = 0;
            };
        } // Support
    } // Core
} // ngx
