class DictionaryItem: public RBNode, public Achor{
protected:
    const char *Key;
    size_t Length;
    uint32_t  Hash;

public:

    DictionaryItem(const char*Key);

    inline int32_t operator - (uint32_t RHash) {

        int32_t Delta = Hash - RHash;

        if (Delta > 0) {
            return 1;
        } else if (Delta < 0) {
            return -1;
        } else {
            return 0;
        }
    }

    virtual int operator - (RBNode &R);
};


class Dictionary : public RBT {
public:
    Dictionary() = default;

    ~Dictionary();

    int AddItem(DictionaryItem &I);

    // might got hash collison, should use prev, next to watch and avoid collision
    DictionaryItem *FindItem(uint32_t Hash);
    DictionaryItem *FindItem(const char *Key);
};

inline int32_t operator- (uint32_t Hash, DictionaryItem &D) {
    return - (D - Hash);
}