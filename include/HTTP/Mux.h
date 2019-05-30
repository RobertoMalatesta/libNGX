#ifndef LIBNGX_HTTP_MUX
#define LIBNGX_HTTP_MUX

#include <string>
#include "HTTP/HTTPConnection.h"
#include "HTTP/HTTPRequest.h"


namespace ngx{
    namespace HTTP {
        class Component {
        public:
            virtual const std::string getType() { return "component"; };
            virtual int reload() = 0;
            virtual int config(std::string key, std::string val) = 0;
            virtual ~Component() = default;
        };

        class Mux : public Component{
        public:
            const std::string getType() final { return "mux"; };
            int reload() override { return -1; };
            int config(std::string key, std::string val) override { return -1; };
            virtual int operator() (HTTPConnection &C, HTTPRequest &R) { return -1; };
            virtual ~Mux() = default;
        };

        class Service : public Component {
        public:
            virtual ~Service() = default;
            virtual int doRequest(HTTPConnection &C, HTTPRequest &Req, HTTPResponse &Res) = 0;
        };

        std::unique_ptr<Component> loadDynamicComponent(std::string file);
    } // HTTP
} // ngx
#endif