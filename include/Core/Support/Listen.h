//===---------------- Listening.h - represent a Connection ---------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file declare the Listening basic class
//
//===-------------------------------------------------------------------------===//
namespace ngx {
    namespace Core {
        namespace Support {
/**
 *  @name Listen
 *
 *  @brief Server Listen
 * */
            class Listen : public EventSubscriber {
            protected:
                int FD;
                Address_t Address;
            public:
                Listen();

                Listen(int FD, Address_t &Addr);

                ~Listen();

                SocketError setPortReuse(unsigned On);

                virtual SocketError bind(const Address_t &Addr) = 0;

                virtual SocketError close();

                virtual SocketError listen(int Backlog);
            };
        } // Support
    } // Core
} // ngx
