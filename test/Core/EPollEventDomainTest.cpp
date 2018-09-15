#include "Core/Core.h"

using namespace ngx::Core;

int EPollEventDomainTest() {

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(8080);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    TCP4Listening Listen((sockaddr *)&server_sockaddr, sizeof(server_sockaddr));
    EPollEventDomain Domain(40960, 4, 31723);

    Listen.SetPortReuse(true).PrintError();
    Listen.Bind().PrintError();
    ForceUSleep(100000);

    return 0;
}