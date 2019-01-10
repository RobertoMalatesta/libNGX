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

    LOG(INFO) << "Init time module start";
    TimeModuleInit();
    LOG(INFO) << "Init time module end";

    HTTPListening Listen(Address);

    EPollEventDomain Domain(2, 32768);

    HTTPServer Server(BUFFER_MEMORY_BLOCK_SIZE, 1, 1, &Domain);

    LOG(INFO) << "Bind() Listening: " << Listen.Bind().GetError();
    LOG(INFO) << "Set NON_BLOCK: " << Listen.SetNonBlock(true).GetError();
    LOG(INFO) << "Set PORT_REUSE: " << Listen.SetPortReuse(true).GetError();
    LOG(INFO) << "Start listening: " << Listen.Listen().GetError();

    LOG(INFO) << "Attach listening to server: " << Server.AttachListening(Listen).GetError();

    RuntimeError Error{0};

    do {

        Error = Server.ServerEventLoop();

        if (Error.GetCode() == 0 && IsInterrupted()) {
            Server.ServerEventLoop();
            break;
        }

    } while (Error.GetCode() == 0);

    LOG(INFO) << "detach listening: " << Server.DetachListening(Listen).GetError();

    return 0;
}