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

class toyMux : public Mux{
public:
    int reload() final { return 0; };
    bool validateConfig() final { return true; }
    int config(std::string key, std::string value) final { return 0; };
    int operator() (HTTPConnection &C, HTTPRequest &R) final {
        return 1234;
    }
};

TEST(HTTPTest, MuxTest) {

    toyMux mux;

}


TEST(HTTPTest, dynamicMuxTest) {

    std::string file = "./libtestMux.so";
    HTTPConnection c;
    HTTPRequest r;
    Mux &mux = reinterpret_cast<Mux &>(*Mux::loadDynamicMux(file));

    std::cout <<  mux(c, r) << std::endl;
}