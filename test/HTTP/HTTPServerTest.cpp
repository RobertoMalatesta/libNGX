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

    LOG(INFO) << "Init time module start";
    TimeModuleInit();
    LOG(INFO) << "Init time module end";

    HTTPListening Listen(SocketAddress);
    HTTPServer Server(POOL_MEMORY_BLOCK_SIZE, 3, EPOLL_EVENT_MAX_CONNECTION, BUFFER_MEMORY_BLOCK_SIZE, 512, 1024);

    LOG(INFO) << "Bind() Listening: " << Listen.Bind().GetError();
    LOG(INFO) << "Set NON_BLOCK: " <<Listen.SetNonBlock(true).GetError();
    LOG(INFO) << "Set PORT_REUSE: " << Listen.SetPortReuse(true).GetError();
    LOG(INFO) << "Start listening: " << Listen.Listen().GetError();

    LOG(INFO) << "Attach listening to server: " << Server.AttachListening(Listen).GetError();

    RuntimeError Error{0};

    do {

        Error = Server.ServerEventProcess();

        if (Error.GetCode() == 0 && IsInterrupted()) {
            Server.ServerEventProcess();
            break;
        }

    } while (Error.GetCode() == 0);

    LOG(INFO) << "detach listening: " << Server.DetachListening(Listen).GetError();

    return 0;
}