
extern int HTTPServerTest();

extern int HTTPConnectionRecycleBinTest();

int HTTPTest() {
    HTTPServerTest();
//    HTTPConnectionRecycleBinTest();
    return 0;
}