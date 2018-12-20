class DictionaryItem: public RBNode, public Achor{
protected:
    const char *Key;
    size_t Length;
    uint32_t  Hash;

public:

    DictionaryItem(const char*Key);

    inline uint32_t GetHash() const { return Hash; }

    inline int32_t operator - (uint32_t RHash) {

        if (Hash > RHash) {
            return 1;
        } else if (Hash < RHash) {
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
    DictionaryItem *FindItem(const char *Key);
};

inline int32_t operator- (uint32_t Hash, DictionaryItem &D) {
    return - (D - Hash);
}
