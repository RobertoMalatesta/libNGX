class HTTPHeader;
class HTTPCoreHeader;

typedef HTTPError (HTTPHeaderProcess)(HTTPCoreHeader &, HTTPRequest &, HTTPHeader&H);

struct HTTPHeader {
    BoundCursor Name;
    BoundCursor Value;
    bool Valid;

    HTTPHeader() : Name(), Value(), Valid(true) {}
};

class HTTPCoreHeader: public DictionaryItem {
protected:
    HTTPCoreHeaderIn Type;
    HTTPHeaderProcess *Setter = nullptr;

    virtual uint32_t HashFn() const;
public:
    HTTPCoreHeader(const char *Key, HTTPCoreHeaderIn HeaderInEnum, HTTPHeaderProcess *HeaderProcess);

    HTTPError Process(HTTPRequest &Request, HTTPHeader &Header);

    const char *GetKey() const { return Key; }

    HTTPCoreHeaderIn GetType() const { return Type; }

    inline bool IsValid() const {
        return Key != nullptr && Setter != nullptr;
    };
};
