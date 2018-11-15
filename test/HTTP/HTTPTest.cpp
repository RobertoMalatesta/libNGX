
extern int HTTPServerTest();

extern int HTTPConnectionRecycleBinTest();

extern int HTTPParseTest();

int HTTPTest() {
    {
        HTTPServerTest();
    }
    int a = 1;
//    HTTPConnectionRecycleBinTest();
//    HTTPParseTest();
    return 0;
}