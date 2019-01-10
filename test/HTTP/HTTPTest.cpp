
#include <cstdio>

extern int HTTPServerTest();

extern int HTTPConnectionCollectorTest();

extern int HTTPParseTest();

int HTTPTest() {
    {
        HTTPServerTest();
    }
//    HTTPConnectionCollectorTest();
//    HTTPParseTest();
    return 0;
}