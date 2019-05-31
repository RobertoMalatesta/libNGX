#include <iostream>
#include <gtest/gtest.h>
//#include "Core/Core.h"
#include "HTTP/HTTP.h"
#include "HTTP/HTTPComponent.h"
using namespace ngx::HTTP;

extern int HTTPConnectionCollectorTest();

extern int HTTPParseTest();

int HTTPTest() {
//    HTTPConnectionCollectorTest();
//    HTTPParseTest();
    return 0;
}


TEST(HTTPTest, MuxTest) {

    std::string file = "./libtestMux.so";
    HTTPConnection c;
    HTTPRequest r;
    Mux &mux = reinterpret_cast<Mux &>(*Mux::loadDynamicMux(file));

    std::cout <<  mux(c, r) << std::endl;
}