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
    Listen.Listen().PrintError();

    Domain.EPollAddListening(&Listen).PrintError();

    while (true) {
        RuntimeError Error = Domain.EventDomainProcess();

        if (Error.GetErrorCode() != 0) {
            Error.PrintError();
        }

        ForceUSleep(500);
    }

    return 0;
}