class DictionaryItem: public RBNode, public Achor{

protected:

    const char *Key;
    uint32_t  Hash = 0;

    virtual int operator - (DictionaryItem &R);
    virtual uint32_t HashFn() const;

public:

    DictionaryItem(const char *Key);

    inline uint32_t GetHash() {

        if (Hash == 0) {
            Hash = HashFn();
        }

        return Hash;
    }

    inline int32_t operator - (uint32_t RHash) {

        if (GetHash() > RHash) {
            return 1;
        } else if (GetHash() < RHash) {
            return -1;
        } else {
            // return strcmp result?
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

    DictionaryItem *FindItem(DictionaryItem Item);
};
