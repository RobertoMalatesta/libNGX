#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::HTTP;

int HTTPServerTest() {

    SocketAddress SocketAddress = {
            .sockaddr_in = {
                    .sin_family = AF_INET,
                    .sin_port = htons(8080),
                    .sin_addr = htonl(INADDR_ANY)
            },
            .SocketLength = sizeof(sockaddr_in)
    };

    TimeModuleInit();

    HTTPListening Listen(SocketAddress);
    HTTPServer Server(POOL_MEMORY_BLOCK_SIZE, 3, EPOLL_EVENT_MAX_CONNECTION, BUFFER_MEMORY_BLOCK_SIZE, 500, 500);

    Listen.SetPortReuse(false).PrintError();
    Listen.Listen().PrintError();

    Server.AttachListening(Listen).PrintError();

    RuntimeError Error{0};
    int i=0;
    do {

        Error = Server.ServerEventProcess();

        if (Error.GetCode() == 0 && IsInterrupted()) {
            Error = Server.ServerEventProcess();
            break;
        }

    } while (Error.GetCode() == 0);

    Server.DetachListening(Listen);

    return 0;
}