#include "Core/Core.h"

#include <iostream>

using namespace std;
using namespace ngx::Core::Support;

int TCPSocketTest() {

    sockaddr_in addr ={
            .sin_family = AF_INET,
            .sin_port = htons(8080),
            .sin_addr = {
                    .s_addr = INADDR_ANY
            }
    };

    Address_t Address(addr);

    TCPListening Listen(Address);

    cout << Listen.SetPortReuse(true).GetError() << endl;
    cout << Listen.Listen().GetError() << endl;

    return 0;
}