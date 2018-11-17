namespace ngx {
    namespace HTTP {

        class HTTPListening : public TCP4Listening {
        protected:

            HTTPServer *ParentServer;
            SocketEventDomain *ParentEventDomain;


            friend class HTTPServer;

            virtual RuntimeError HandleEventDomain(uint32_t EventType);

        public:

            HTTPListening(struct SocketAddress &SocketAddress);

            ~HTTPListening() = default;
        };
    }
}
