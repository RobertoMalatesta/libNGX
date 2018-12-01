#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

DictionaryItem::DictionaryItem(const char *Key) {

    if (Key == nullptr) {
        return;
    }

    this -> Key = Key;
    Length = strlen(Key);
}

int DictionaryItem::operator-(RBNode &R) {

    return *this - ((DictionaryItem &)R).Hash;
}

Dictionary::~Dictionary() {

    for (RBNode *It = this->Begin(); It ; It = Next(It)) {
        Erase(It);
    }
}

int Dictionary::AddItem(DictionaryItem &I) {
    Insert(&I);
    return 0;
}

DictionaryItem *Dictionary::FindItem(uint32_t Hash) {

    RBNode *Place = Root;

    while (Place) {

        int Result = Hash - *((DictionaryItem*)Place);

        if (Result < 0) {
            Place = Place->GetLeft();
        } else if (Result > 0){
            Place = Place->GetRight();
        } else {
            return (DictionaryItem *)Place;
        }
    }

    return nullptr;
}

DictionaryItem *Dictionary::FindItem(const char *Key) {

    size_t Length = strlen(Key);
    uint32_t Hash = 0 ^ Length;

    for (size_t i=0; i<Length; i++) {
        SimpleHash(Hash, Key[i]);
    }

    return FindItem(Hash);
}
