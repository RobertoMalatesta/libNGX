template<class ItemType>
class AllocatorBuild {
private:
    MemAllocator *Allocator;
public:
    AllocatorBuild(MemAllocator *Allocator = nullptr) {
        this->Allocator = Allocator;
    }

    int Build(ItemType *&Item) {

        void *TempPointer;

        Item = nullptr;

        if (Allocator != nullptr) {
            TempPointer = Allocator->Allocate(sizeof(ItemType));
        } else {
            TempPointer = malloc(sizeof(ItemType));
        }

        if (TempPointer == nullptr) {
            return -1;
        }

        Item = new(TempPointer) ItemType();

        return 0;
    };

    int Destroy(ItemType *&Item) {

        if (nullptr == Item) {
            return 0;
        }

        if (Allocator != nullptr) {
            Allocator->Free((void *&) Item);
        } else {
            free((void *) Item);
            Item = nullptr;
        }

        return 0;
    };
};