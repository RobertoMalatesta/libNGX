#ifndef LIBNGX_HTTP_MUX
#define LIBNGX_HTTP_MUX

#include <string>
#include "HTTP/HTTPConnection.h"
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPResponse.h"
namespace ngx{
    namespace HTTP {
        typedef void *(*loadComponentFn)();

        static const std::string muxSym        = "loadMux";
        static const std::string serviceSym    = "loadService";

        class Component {
        protected:
            void *h_dl;
            static const std::string type;
            static Component *loadDynamicComponent(std::string &file, std::string &sym);

        public:
            Component() : h_dl(nullptr) {};
            virtual const std::string getType() { return Component::type; };
            virtual ~Component();
            virtual int reload() = 0;
            virtual bool validateConfig() = 0;
            virtual int config(std::string key, std::string val) = 0;
        };

        class Mux : public Component{
        protected:
            static const std::string type;

        public:
            ~Mux() = default;
            const std::string getType() final { return type; };
            virtual int operator() (HTTPConnection &C, HTTPRequest &R) = 0;
            static Mux *loadDynamicMux(std::string &file);
        };

        class Service : public Component {
        protected:
            static const std::string type;
            static const std::string name;

        public:
            ~Service() = default;
            const std::string getType() override { return type; };
            virtual int operator() (HTTPConnection &C, HTTPRequest &Req, HTTPResponse &Res) = 0;
            static Service *loadDynamicService(std::string &file);
        };
    } // HTTP
} // ngx
#endif