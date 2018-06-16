namespace ngx::Core {
    class ListIterator {

        private:
            ListCarriage *CurrentCarriage;
            uint32_t CurrentIndex;
            uint32_t Size;

        public:
            ListIterator( ListCarriage *Carriage, uint32_t Index, uint32_t Size) {
                CurrentCarriage = Carriage;
                CurrentIndex = Index;
                this -> Size = Size;
            }
            void *Next() {
                if (nullptr != CurrentCarriage) {

                    if (CurrentCarriage -> NumberOfElements == CurrentIndex) {
                        CurrentCarriage = CurrentCarriage -> Next;
                        CurrentIndex = 0;
                    }

                    if (nullptr != CurrentCarriage) {
                        return &(CurrentCarriage -> PointerToElements [Size * (CurrentIndex++)]);
                    }
                }
                return nullptr;
            }
    };

    class ListCarriage {

        private:
            void *PointerToElements;
            uint32_t NumberOfElements;
            ListCarriage *Next;

        public:
            void *operator new(size_t Size, void * PointerToAllocator);

            ListCarriage(uint32_t NumberOfElements) {
                this -> PointerToElements = this + sizeof(ListCarriage);
                this -> NumberOfElements = NumberOfElements;
                this -> Next = nullptr;
            }
    };

    class List{
        private:
            ListCarriage *LastOne;
            ListCarriage TheCarriage;
            size_t Size;
            uint32_t NAllocated;
            MemAllocator *Allocator;
    };

}