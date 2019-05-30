#include <iostream>
#include <gtest/gtest.h>
//#include "Core/Core.h"
#include "HTTP/HTTP.h"
#include "HTTP/Mux.h"
using namespace ngx::HTTP;

extern int HTTPConnectionCollectorTest();

extern int HTTPParseTest();

int HTTPTest() {
//    HTTPConnectionCollectorTest();
//    HTTPParseTest();
    return 0;
}

#include <dlfcn.h>

TEST(HTTPTest, MuxTest) {

    Component *c = new Mux();
    std::cout << c->getType() << std::endl;
    delete c;

    void *handle = dlopen("./libtestMux.so",RTLD_NOW | RTLD_GLOBAL);
    ASSERT_FALSE(handle== nullptr);

    typedef void *(*loadFn)();

    loadFn fn = reinterpret_cast<loadFn >(dlsym(handle, "loadMux"));

    void *target = fn();

    Mux *tMux = reinterpret_cast<Mux *>(target);
    HTTPConnection conn;
    HTTPRequest r;
    std::cout<< tMux->operator() (conn, r) << std::endl;

    delete tMux;

    dlclose(handle);
}