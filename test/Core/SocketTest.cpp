#include "Core/Core.h"

#include <iostream>

using namespace ngx::Core;
using namespace std;
int TCPSocketTest() {

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(8080);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    TCP4Listening Listen((sockaddr *)&server_sockaddr, sizeof(server_sockaddr));

    cout<<Listen.SetPortReuse(true).GetErrorMessage()<<endl;
    cout<<Listen.Bind().GetErrorMessage()<<endl;

    return 0;
}