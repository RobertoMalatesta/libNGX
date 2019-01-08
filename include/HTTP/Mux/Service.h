class Service : public DictionaryItem {

public:
    Service(const char *Key) : DictionaryItem(Key) {};

    virtual HTTPError HandleRequest(HTTPRequest &R) = 0;
};
