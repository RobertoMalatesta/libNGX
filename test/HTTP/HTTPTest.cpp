
#include <zconf.h>
#include <cstdio>
extern int HTTPServerTest();

extern int HTTPConnectionRecycleBinTest();

extern int HTTPParseTest();

int HTTPTest() {
    {
//        HTTPServerTest();
    }
//    HTTPConnectionRecycleBinTest();
    HTTPParseTest();
    return 0;
}