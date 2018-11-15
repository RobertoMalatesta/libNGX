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

    TCP4Listening Listen(SocketAddress);
    HTTPServer Server(40960, 3, 31728, 40960, 1024, 1024);

    Listen.SetPortReuse(false).PrintError();
    Listen.Listen().PrintError();

    Server.EnqueueListening(&Listen).PrintError();

    RuntimeError Error{0};

    do {
        Error = Server.HTTPServerEventProcess();
    } while (Error.GetCode() == 0 && !IsInterrupted());

    return 0;
}