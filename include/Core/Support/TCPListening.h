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
class TCPListening : public Listening {
public:
    TCPListening(Address_t &Address);

    ~TCPListening();

    virtual SocketError Bind();

    virtual RuntimeError HandleDomainEvent(EventType Type) {
        return {0};
    };
};
