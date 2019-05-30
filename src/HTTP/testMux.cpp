#include "HTTP/Mux.h"

using namespace ngx::HTTP;

class Mux1 : public Mux{
public:
    virtual int operator() (HTTPConnection &C, HTTPRequest &R) {
        return 1234;
    }
};

extern "C" void *loadMux() {
    return reinterpret_cast<void *>(new Mux1());
}
