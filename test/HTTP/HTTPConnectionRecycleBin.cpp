#include "HTTP/HTTP.h"
#include <stdio.h>

using namespace ngx::HTTP;

static void Stage1() {
    int Random = 0;
    HTTPConnection *C;
    HTTPConnectionRecycleBin RecycleBin(100);
    srandom(getpid());

    struct SocketAddress SocketAddress = {
            .sockaddr_in = {
                    .sin_family = AF_INET,
                    .sin_port = htons(8080),
                    .sin_addr = htonl(INADDR_ANY)
            },
            .SocketLength = sizeof(sockaddr_in)
    };

    int j = 0;
    HTTPConnection *Connections[10];

    for (int i = 0; i < 102400000; i++) {
        if (RecycleBin.Get(C, -1, SocketAddress) == 0) {
            Random = 1 + (int) (10.0 * (random() / (RAND_MAX + 1.0)));

            if (j == 10) {
                for (int k = 0; k < 10; k++) {
                    RecycleBin.Put(Connections[k]);
                }
                j = 0;
            }

            if (Random > 2) {
                RecycleBin.Put(C);
            } else {
                Connections[j++ % 10] = C;
            }
        }
    }
    printf("stage1\n");
    getchar();
}

int HTTPConnectionRecycleBinTest() {

    Stage1();
    printf("stage2\n");
    getchar();

    return 0;
}
