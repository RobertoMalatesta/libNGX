#include "Http/Http.h"

using namespace ngx::Core;
using namespace ngx::Http;

HttpConnection::HttpConnection(struct sockaddr *SocketAddress, socklen_t SocketLength):
        TCP4Connection(SocketAddress, SocketLength){
    Lock.Unlock();
    OnEvent = & HttpConnection::OnEventFunc;
}

HttpConnection::HttpConnection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength) :
        TCP4Connection(SocketFd, SocketAddress, SocketLength){
    Lock.Unlock();
    OnEvent = & HttpConnection::OnEventFunc;
}

char buffer[BUFSIZ];

void HttpConnection::OnEventFunc(void *Argument, ThreadPool *TPool) {

    HttpServer *Server;
    EPollEventDomain *EventDomain;
    EPollEventDomainArgument *EventDomainArguments;
    HttpConnection *Connection;

    if (Argument == nullptr) {
        return;
    }

    EventDomainArguments = static_cast<EPollEventDomainArgument *>(Argument);
    EventDomain = EventDomainArguments->EventDomain;
    Connection = static_cast<HttpConnection *>(EventDomainArguments->UserArguments[1].Ptr);
    Server = static_cast<HttpServer *>(EventDomainArguments->UserArguments[0].Ptr);

    if (EventDomain == nullptr || Connection == nullptr) {
        return;
    }

    if (EventDomainArguments->UserArguments[2].UInt & ET_CONNECTED) {
        EventDomain->AttachSocket(Connection, SOCK_READ_WRITE_EVENT);
        Server->AttachConnection(Connection);
    }

    if (EventDomainArguments->UserArguments[2].UInt & ET_READ) {
        EventDomain->DetachSocket(Connection, SOCK_READ_EVENT);
        ssize_t n = recv(Connection->GetSocketFD(), buffer, BUFSIZ, 0);
        cout<< "Read: "<< Connection->GetSocketFD() << "-" << n<< ": " << buffer << endl;
    }

    if (EventDomainArguments->UserArguments[2].UInt & ET_WRITE) {
        EventDomain->DetachSocket(Connection, SOCK_WRITE_EVENT);
        cout<< "Write: "<< Connection->GetSocketFD() << endl;
    }
    printf("event end: %p, %p\n", EventDomain, Connection);
}

void HttpConnection::Reset() {



}