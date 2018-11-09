namespace ngx {
    namespace Core {
        namespace MetaClass {
            class Ref {
                virtual uint32_t IncRef() = 0;

                virtual uint32_t DecRef() = 0;
            };
        }
    }
}
