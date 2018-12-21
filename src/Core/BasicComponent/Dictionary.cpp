#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

DictionaryItem::DictionaryItem(const char *Key) {

    if (Key == nullptr) {
        return;
    }

    this -> Key = Key;
}

uint32_t DictionaryItem::HashFn() const {

    size_t Length = strlen(Key);

    uint32_t TempHash = 0 ^ Length;

    for (size_t i=0; i<Length; i++) {
        SimpleHash(TempHash, Key[i]);
    }

    return TempHash;
}

int DictionaryItem::operator-(DictionaryItem &R) {

    int HashDiff = *this - R.GetHash();

    return  HashDiff == 0? strcmp(Key, R.Key): HashDiff;
}

int DictionaryItem::operator-(RBNode &R) {
    return *this - ((DictionaryItem &)R);
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

        int Result = Hash - ((DictionaryItem*)Place)->GetHash();

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

DictionaryItem *Dictionary::FindItem(DictionaryItem Item) {

    RBNode *Place = Root;

    while (Place) {

        int Result = Item - *(Place);

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
