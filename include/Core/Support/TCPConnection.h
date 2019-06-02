//===------------ TCPConnection.h - represent a TCPConnection ------*- C++ -*-===//
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
#ifndef LIBNGX_CORE_SUPPORT_TCPCONN
#define LIBNGX_CORE_SUPPORT_TCPCONN

namespace ngx {
namespace Core {
namespace Support {

/**
 *  @name TCPConnection
 *
 *  @brief Represent a tcp connection
 * */
class TCPConnection : public Connection {
public:
    TCPConnection();

    TCPConnection(int FD, Address_t &Addr) : Connection(FD, Addr) {}

    virtual SocketError connect(Address_t &Address);
};

} // Support
} // Core
} // ngx

#endif
