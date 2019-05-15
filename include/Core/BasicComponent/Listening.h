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

/**
 *  @name Listening
 *
 *  @brief Server Listening
 * */
class Listening : public Socket {
protected:
    int Backlog = 32768;
public:
    Listening();

    Listening(Address_t &Address);

    Listening(int SocketFD, Address_t &Address);

    SocketError SetPortReuse(bool On);

    virtual SocketError Bind() = 0;

    virtual SocketError Listen();
};
