namespace ngx {
    namespace Core {
        namespace MetaClass {

            template<class T, size_t ALIGNMENT = 4096>
            class AlignBuild {
            public:
                static int Build(T *&Item, size_t Size) {

                    void *TempPointer;

                    int Code = posix_memalign(&TempPointer, ALIGNMENT, Size);
                    Item = nullptr;

                    if (Code != 0) {
                        return Code;
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