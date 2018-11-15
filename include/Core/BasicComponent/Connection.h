//===- Connection.h - represent a Connection -----*- C++ -*-===//
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

    Connection(struct SocketAddress &SocketAddress);

    Connection(int SocketFd, struct SocketAddress &SocketAddress);

    virtual SocketError Connect() {
        return SocketError(EINVAL, "Method not implement!");
    };

    virtual SocketError Close() {
        return SocketError(EINVAL, "Method not implement!");
    };
};


class TCP4Connection : protected Connection {
public:
    TCP4Connection(struct SocketAddress &SocketAddress);

    TCP4Connection(int SocketFd, struct SocketAddress &SocketAddress);

    virtual SocketError Connect();

    virtual SocketError Close();

    SocketError SetNoDelay(bool Open);
};
