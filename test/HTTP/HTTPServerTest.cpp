#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::Core::Arch::Linux;
using namespace ngx::HTTP;

int HTTPServerTest() {


    SocketAddress Address;

    Address.sockaddr_in = {
            .sin_family = AF_INET,
            .sin_port = htons(8080),
            .sin_addr = htonl(INADDR_ANY),
    };

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