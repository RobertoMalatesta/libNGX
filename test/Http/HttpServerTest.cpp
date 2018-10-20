#include "Http/Http.h"

using namespace ngx::Core;
using namespace ngx::Http;

int HttpServerTest() {

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(8080);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    SocketAddress SocketAddress = {.sockaddr_in = server_sockaddr};

    TCP4Listening Listen(SocketAddress, sizeof(server_sockaddr));
    HttpServer Server(40960, 4, 31728, 1024, 1024);

    Listen.SetPortReuse(false).PrintError();
    Listen.Listen().PrintError();

    Server.EnqueueListening(&Listen).PrintError();

    int Count = 500000;

    while (Count -- > 0) {
        RuntimeError Error = Server.HttpServerEventProcess();
        Error.PrintError();
        ForceUSleep(100000);
    }

    return 0;
}