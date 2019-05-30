#include "HTTP/HTTPError.h"
#include "HTTP/HTTPCoreHeader.h"

using namespace ngx::HTTP;

HTTPCoreHeader::HTTPCoreHeader(const char *Key, HTTPCoreHeaderIn HeaderInEnum, HTTPHeaderProcess *HeaderProcess)
        : DictionaryItem(Key) {

    if (Key == nullptr) {
        return;
    }

    Type = HeaderInEnum, Setter = HeaderProcess;
}

uint32_t HTTPCoreHeader::HashFn() const {

    size_t Length = strlen(Key);
    uint32_t TempHash = 0 ^Length;

    for (size_t i = 0; i < Length; i++) {
        SimpleHash(TempHash, lower_case[Key[i]]);
    }

    return TempHash;
}

HTTPError HTTPCoreHeader::Process(HTTPRequest &Request, HTTPHeader &Header) {

    if (Setter == nullptr) {
        return {EINVAL, "invalid header process"};
    }
    return Setter(*this, Request, Header);
}
