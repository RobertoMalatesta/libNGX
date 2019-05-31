#include "HTTP/HTTPComponent.h"

using namespace ngx::HTTP;

class toyMux : public Mux{
public:
    int reload() final { return 0; };
    bool validateConfig() final { return true; }
    int config(std::string key, std::string value) final { return 0; };
    int operator() (HTTPConnection &C, HTTPRequest &R) final {
        return 1234;
    }
};

extern "C" void *loadMux() {
    return static_cast<void *>(new toyMux());
}
