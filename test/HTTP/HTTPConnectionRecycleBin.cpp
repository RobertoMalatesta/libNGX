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

    HTTPConnection *C;
    HTTPConnectionRecycleBin RecycleBin(1);

    for (int i = 0; i < 1024000; i++) {
        if (RecycleBin.Get(C, -1, SocketAddress) == 0) {
            RecycleBin.Put(C);
        }
    }

    return 0;
}
