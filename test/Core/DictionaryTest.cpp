#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

class DictItem : public DictionaryItem {
public:
    uint32_t Value;

    DictItem(const char *Key, uint32_t Value) : DictionaryItem(Key), Value(Value) {}
};

int DictionaryTest() {

    Dictionary D;

    DictItem I1("Item1", 1);
    DictItem I2("Item2", 2);
    DictItem I3("Item3", 3);
    DictItem I4("Item4", 4);
    DictItem I5("Item5", 5);
    DictItem I6("Item6", 6);
    DictItem I7("Item7", 7);
    DictItem I8("Item8", 8);
    DictItem I9("Item9", 9);
    DictItem I10("Item10", 10);
    DictItem I11("Item11", 11);
    DictItem I12("Item12", 12);

    D.Insert(I1);
    D.Insert(I2);
    D.Insert(I3);
    D.Insert(I4);
    D.Insert(I5);
    D.Insert(I6);
    D.Insert(I7);
    D.Insert(I8);
    D.Insert(I9);
    D.Insert(I10);
    D.Insert(I11);
    D.Insert(I12);

    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item1"}))->Value);
    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item2"}))->Value);
    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item3"}))->Value);
    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item4"}))->Value);
    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item5"}))->Value);
    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item6"}))->Value);
    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item7"}))->Value);
    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item8"}))->Value);
    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item9"}))->Value);
    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item10"}))->Value);
    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item11"}))->Value);
    printf("result: %u\n", ((DictItem *) D.Find(DictionaryItem{"Item12"}))->Value);

    return 0;
}
