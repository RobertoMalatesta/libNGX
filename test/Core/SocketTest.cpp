#include "Core/Core.h"

using namespace ngx::Core;

int TCPSocketTest() {



    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(8080);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    TCP4Listening Listen((sockaddr *)&server_sockaddr, sizeof(server_sockaddr));
    Listen.SetPortReuse(true);
    Listen.Bind();

    printf("%d\n", Listen.GetSocketFD());

    return 0;

}