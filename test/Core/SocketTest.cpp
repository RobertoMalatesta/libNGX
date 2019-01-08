#include "Core/Core.h"

#include <iostream>

using namespace ngx::Core;
using namespace std;

int TCPSocketTest() {

    SocketAddress Address;

    Address.sockaddr_in = {
            .sin_family = AF_INET,
            .sin_port = htons(8080),
            .sin_addr = htonl(INADDR_ANY),
    };

    TCPListening Listen(Address);

    cout << Listen.SetPortReuse(true).GetError() << endl;
    cout << Listen.Listen().GetError() << endl;

    return 0;
}