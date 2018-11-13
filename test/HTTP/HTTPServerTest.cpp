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

    TCP4Listening Listen(SocketAddress);
    HTTPServer Server(40960, 4, 31728, 40960, 1024, 1024);

    Listen.SetPortReuse(false).PrintError();
    Listen.Listen().PrintError();

    Server.EnqueueListening(&Listen).PrintError();

    TimeModuleInit();

    RuntimeError Error{0};

    do {
        Error = Server.HTTPServerEventProcess();

    } while (Error.GetCode() == 0);

    return 0;
}