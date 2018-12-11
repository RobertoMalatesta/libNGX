class HTTPHeader;
class HTTPCoreHeader;

typedef HTTPError (HTTPHeaderProcess)(HTTPCoreHeader &, HTTPRequest &, HTTPHeader&H);

const u_char LowerCase[] =
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0-\0\0" "0123456789\0\0\0\0\0\0"
        "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
        "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

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
public:
    HTTPCoreHeader(const char *Key, HTTPCoreHeaderIn HeaderInEnum, HTTPHeaderProcess *HeaderProcess);

    HTTPError Process(HTTPRequest &Request, HTTPHeader &Header);

    inline bool IsValid() const {
        return Key != nullptr && Setter != nullptr;
    };

};
