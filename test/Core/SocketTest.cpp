#include "Core/Core.h"

#include <iostream>

using namespace ngx::Core;
using namespace std;
int TCPSocketTest() {

    SocketAddress SocketAddress = {
            .sockaddr_in = {
                    .sin_family = AF_INET,
                    .sin_port = htons(8080),
                    .sin_addr = htonl(INADDR_ANY)
            },
            .SocketLength = sizeof(sockaddr_in)
    };

    TCP4Listening Listen(SocketAddress);

    cout<<Listen.SetPortReuse(true).GetErrorString()<<endl;
    cout<<Listen.Listen().GetErrorString()<<endl;

    return 0;
}