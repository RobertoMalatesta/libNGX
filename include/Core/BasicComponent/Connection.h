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


class Connection : public Socket {
protected:
public:
    Connection();

    Connection(SocketAddress &Address);

    Connection(int SocketFD, SocketAddress &Address);

    virtual SocketError Connect() = 0;
};


class TCPConnection : public Connection {
public:
    TCPConnection(SocketAddress &Address);

    TCPConnection(int SocketFD, SocketAddress &Address);

    virtual SocketError Connect();
};
