#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

class DictItem: public DictionaryItem {
public:
    uint32_t Value;
    DictItem(const char *Key, uint32_t Value):DictionaryItem(Key), Value(Value){}
};

int DictionaryTest() {

    Dictionary D;

    DictItem I1("Item1", 1);
    DictItem I2("Item2", 2);
    DictItem I3("Item3", 3);
    DictItem I4("Item4", 4);
    DictItem I5("Item5", 5);
    DictItem I6("Item6", 6);

    D.AddItem(I1);
    D.AddItem(I2);
    D.AddItem(I3);
    D.AddItem(I4);
    D.AddItem(I5);
    D.AddItem(I6);

    printf("result: %u\n", ((DictItem *)D.FindItem(DictionaryItem{"Item1"}))->Value);
    printf("result: %u\n", ((DictItem *)D.FindItem(DictionaryItem{"Item2"}))->Value);
    printf("result: %u\n", ((DictItem *)D.FindItem(DictionaryItem{"Item3"}))->Value);
    printf("result: %u\n", ((DictItem *)D.FindItem(DictionaryItem{"Item4"}))->Value);
    printf("result: %u\n", ((DictItem *)D.FindItem(DictionaryItem{"Item5"}))->Value);
    printf("result: %u\n", ((DictItem *)D.FindItem(DictionaryItem{"Item6"}))->Value);

    return 0;
}
