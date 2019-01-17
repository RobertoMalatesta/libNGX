struct HTTPCoreHeader : public DictionaryItem {

    HTTPCoreHeaderIn Type;
    HTTPHeaderProcess *Setter = nullptr;

    virtual uint32_t HashFn() const;

    HTTPCoreHeader(const char *Key, HTTPCoreHeaderIn HeaderInEnum, HTTPHeaderProcess *HeaderProcess);

    inline const char *GetKey() const { return Key; }

    inline HTTPCoreHeaderIn GetType() const { return Type; }

    HTTPError Process(HTTPRequest &Request, HTTPHeader &Header);
};
