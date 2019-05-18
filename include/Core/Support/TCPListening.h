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

/**
 *  @name TCPListening
 *
 *  @brief Server tcp listening
 * */
class TCPListening : public Listen {
public:
    TCPListening();

    TCPListening(int FD, Address_t &Addr) : Listen(FD, Addr) {};

    virtual SocketError bind(const Address_t &Addr) override;
};
