namespace ngx {
    namespace Core {
        namespace MetaClass {

            template<class T, int ALIGNMENT = 4096>
            class BuildAlign {
            public:
                static int Build(T *&Item, size_t Size) {

                    void *TempPointer = valloc(Size);

                    Item = nullptr;

                    if (nullptr == TempPointer) {
                        return errno;
                    }

                    Item = new(TempPointer) T(Size);
                    return 0;
                };

                static int Destroy(T *&Item) {

                    if (nullptr == Item) {
                        return 0;
                    }

                    free((void *) Item);

                    Item = nullptr;
                    return 0;
                };
            };
        }
    }
}