#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::Core::Target::Linux;
using namespace ngx::HTTP;

int HTTPServerTest() {

    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(8080),
            .sin_addr = {
                    .s_addr=INADDR_ANY
            }
    };

    Address_t Address(addr);

    TimeModuleInit();

    HTTPListening Listen(Address);

    EPollEventDomain Domain(2, 32768);

    HTTPServer Server(BUFFER_MEMORY_BLOCK_SIZE, 1, 1, &Domain);

    Listen.Bind().GetError();
    Listen.SetNonBlock(true).GetError();
    Listen.SetPortReuse(true).GetError();
    Listen.Listen().GetError();

    Server.AttachListening(Listen).GetError();

    RuntimeError Error{0};

    do {

        Error = Server.ServerEventLoop();

        if (Error.GetCode() == 0 && IsInterrupted()) {
            Server.ServerEventLoop();
            break;
        }

    } while (Error.GetCode() == 0);

    Server.DetachListening(Listen).GetError();

    return 0;
}