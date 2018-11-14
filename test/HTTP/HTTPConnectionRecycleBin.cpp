#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

int HTTPConnectionRecycleBinTest() {

    struct SocketAddress SocketAddress = {
            .sockaddr_in = {
                    .sin_family = AF_INET,
                    .sin_port = htons(8080),
                    .sin_addr = htonl(INADDR_ANY)
            },
            .SocketLength = sizeof(sockaddr_in)
    };

    HTTPConnectionRecycleBin RecycleBin(4096);
    HTTPConnection *C;

    std::vector<HTTPConnection *> Connections;

    for (int i = 0; i < 102400; i++) {
        if (RecycleBin.Get(C, -1, SocketAddress) == 0) {
            if (random() >= (RAND_MAX >> 2)) {
                RecycleBin.Put(C);
            } else {
                Connections.push_back(C);
            }
        }
    }

    for (HTTPConnection *Conn : Connections) {
        RecycleBin.Put(Conn);
    }

    Connections.clear();
    return 0;
}
