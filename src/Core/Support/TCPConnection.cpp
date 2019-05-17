#include "Core/Core.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>

using namespace ngx::Core::Support;

TCPConnection::TCPConnection() :Address(){
    FD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TCPConnection::TCPConnection(int FD, Address_t &Address) : Connection(FD, Address) {
}


SocketError TCPConnection::connect(Address_t &Address) {

    if (FD != -1) {
        return {EALREADY, "TCPConnection is already open!"};
    } else if (Address.Addr4.sa_family==AF_UNSPEC){
        return {EBADF, "connect() failed, bad socket address"};
    }

    if(::connect(FD, &Address.Addr4, sizeof(Address))!=0) {
        return {errno, "connect() failed, ::connect() failure"};
    }

    return {0};
}
