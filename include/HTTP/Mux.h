namespace ngx{
    namespace HTTP {
        class Mux {
        public:
            virtual int operator() (HTTPConnection &C, HTTPRequest &R) = 0;
        };

        class Component {
        public:
            virtual int hotReload() = 0;
            virtual int config(std::string key, std::string val) = 0;
            virtual int doRequest(HTTPConnection &C, HTTPRequest &Req, HTTPResponse &Res) = 0;
        };

        std::unique_ptr<Mux> loadDynamicMux(std::string file);
    } // HTTP
} // ngx
