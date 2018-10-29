#include "Http/Http.h"

using namespace ngx::Http;

int HttpConnectionRecyclerTest() {

    struct SocketAddress SocketAddress = {
            .sockaddr_in = {
                    .sin_family = AF_INET,
                    .sin_port = htons(8080),
                    .sin_addr = htonl(INADDR_ANY)
            },
            .SocketLength = sizeof(sockaddr_in)
    };

    HttpConnectionRecycler Recycler(4096, 102400, 5120);
    HttpConnection *C;

    std::vector<HttpConnection *> Connections;

    for (int i = 0; i < 102400; i++) {
        C = Recycler.Get(-1, SocketAddress);

        if (random() >= (RAND_MAX >> 2)) {
            Recycler.Put(C);
        } else {
            Connections.push_back(C);
        }
    }

    for (HttpConnection *Conn : Connections) {
        Recycler.Put(Conn);
    }

    Connections.clear();
    return 0;
}
