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

/**
 *  @name TCPConnection
 *
 *  @brief Represent a tcp connection
 * */
class TCPConnection : public Connection {
public:
    TCPConnection(SocketAddress &Address);

    TCPConnection(int SocketFD, SocketAddress &Address);

    virtual SocketError Connect();
};
