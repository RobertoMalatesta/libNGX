//===---------------- TCPListening.h - represent a Connection ------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file declare the TCPListening class
//
//===-------------------------------------------------------------------------===//

#ifndef LIBNGX_CORE_SUPPORT_TCPLISTEN
#define LIBNGX_CORE_SUPPORT_TCPLISTEN

namespace ngx {
namespace Core {
namespace Support {

/**
 *  @name TCPListening
 *
 *  @brief Server tcp listening
 * */
class TCPListen : public Listen {
public:
    TCPListen() : Listen() {
        FD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    };

    TCPListen(int FD, Address_t &Addr) : Listen(FD, Addr) {};

    virtual SocketError bind(const Address_t &Addr) override;
};

} // Support
} // Core
} // ngx
#endif
