template<class ItemType>
class AllocatorBuild {
private:
    Allocator *BackendAllocator;
public:
    AllocatorBuild(Allocator *Allocator = nullptr) {
        this->BackendAllocator = Allocator;
    }

    int Build(ItemType *&Item) {

        void *TempPointer;

        Item = nullptr;

        if (BackendAllocator != nullptr) {
            TempPointer = BackendAllocator->Allocate(sizeof(ItemType));
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

        if (BackendAllocator != nullptr) {
            Item->~ItemType();
            BackendAllocator->Free((void *&) Item);
        } else {
            free((void *) Item);
            Item = nullptr;
        }

        return 0;
    };
};