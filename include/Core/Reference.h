namespace ngx::Core {

    class Reference {
        protected:
            std::atomic_uint32_t Counter = {0};
        public:
            uint32_t GetReference();
            uint32_t PutReference();
            uint32_t ReferenceCount();
            void ClearReference();
    };
}