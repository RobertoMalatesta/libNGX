#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPCoreHeader::HTTPCoreHeader(const char *Key, HTTPCoreHeaderIn  HeaderInEnum, HTTPHeaderProcess *HeaderProcess): DictionaryItem(Key){

    if (Key == nullptr) {
        return;
    }

    HashFn();

    Type = HeaderInEnum, Setter = HeaderProcess;
}

void HTTPCoreHeader::HashFn() {

    size_t Length = strlen(Key);
    Hash = 0 ^ Length;

    for (size_t i=0; i<Length; i++) {
        SimpleHash(Hash, LowerCase[Key[i]]);
    }
}

HTTPError HTTPCoreHeader::Process(HTTPRequest &Request, HTTPHeader &Header) {

    if (Setter == nullptr) {
        return {EINVAL, "invalid header process"};
    }
    return Setter(*this, Request, Header);
}
