#include "Core/Core.h"

#include <iostream>

using namespace ngx::Core;
using namespace std;
int TCPSocketTest() {

    struct sockaddr_in server_sockaddr;
    SocketAddress SocketAddress;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(8080);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    SocketAddress.sockaddr_in = server_sockaddr;

    TCP4Listening Listen(SocketAddress, sizeof(server_sockaddr));

    cout<<Listen.SetPortReuse(true).GetErrorString()<<endl;
    cout<<Listen.Listen().GetErrorString()<<endl;

    return 0;
}