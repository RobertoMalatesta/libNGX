namespace ngx::Core {


    class ListCarriage {

    private:
        u_char *PointerToElements;
        uint32_t NumberOfElements;
        ListCarriage *Next;

    public:
        void *operator new(size_t Size, void *PointerToAllocator);

        ListCarriage(uint32_t NumberOfElements) {
            this->PointerToElements = (u_char *) this + sizeof(ListCarriage);
            this->NumberOfElements = NumberOfElements;
            this->Next = nullptr;
        }

        friend class ListIterator;
    };

    class List {
    private:
        ListCarriage *LastOne;
        ListCarriage TheCarriage;
        size_t Size;
        uint32_t NAllocated;
        MemAllocator *Allocator;
    };

    class ListIterator {

    private:
        ListCarriage *CurrentCarriage;
        uint32_t CurrentIndex;
        uint32_t Size;

    public:
        ListIterator(ListCarriage *Carriage, uint32_t Index, uint32_t Size) {
            CurrentCarriage = Carriage;
            CurrentIndex = Index;
            this->Size = Size;
        }

        void *Next();
    };
}