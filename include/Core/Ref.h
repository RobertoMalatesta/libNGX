namespace ngx::Core {

    class Ref {
        protected:
            int64_t  RefCount;
        public:
            void Get() {RefCount+=1;};
            void Put() {RefCount-=1;};
    };
}