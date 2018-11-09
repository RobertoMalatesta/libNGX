namespace ngx {
    namespace Core {
        class Reference : Ref {
        protected:
            std::atomic_uint32_t Counter = {0};
        public:

            virtual uint32_t IncRef();

            virtual uint32_t DecRef();

            virtual uint32_t RefCount();

            virtual void ClearRef();
        };
    }
}