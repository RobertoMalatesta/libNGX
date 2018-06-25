#include "Core/Core.h"

using namespace ngx::Core;

void *ListIterator::Next() {
    if (nullptr != CurrentCarriage) {

        if (CurrentCarriage -> NumberOfElements == CurrentIndex) {
            CurrentCarriage = CurrentCarriage -> Next;
            CurrentIndex = 0;
        }

        if (nullptr != CurrentCarriage) {
            return ((char *)(CurrentCarriage -> PointerToElements) +(Size * (CurrentIndex++)));
        }
    }
    return nullptr;
}