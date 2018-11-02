#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

int HTTPConnectionRecyclerTest() {

    struct SocketAddress SocketAddress = {
            .sockaddr_in = {
                    .sin_family = AF_INET,
                    .sin_port = htons(8080),
                    .sin_addr = htonl(INADDR_ANY)
            },
            .SocketLength = sizeof(sockaddr_in)
    };

    HTTPConnectionRecycler Recycler(4096, 102400, 5120);
    HTTPConnection *C;

    std::vector<HTTPConnection *> Connections;

    for (int i = 0; i < 102400; i++) {
        C = Recycler.Get(-1, SocketAddress);

        if (random() >= (RAND_MAX >> 2)) {
            Recycler.Put(C);
        } else {
            Connections.push_back(C);
        }
    }

    for (HTTPConnection *Conn : Connections) {
        Recycler.Put(Conn);
    }

    Connections.clear();
    return 0;
}
