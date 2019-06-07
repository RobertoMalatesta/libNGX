#include <cerrno>
#include "HTTP/common_header.h"
#include "HTTP/HTTPError.h"
#include "HTTP/HTTPHeader.h"
using namespace std;
using namespace ngx::HTTP;
const char BrokenHeaderErrorString[] = "broken header in buffer";
const char NoMoreHeaderErrorString[] = "no more header";
const char InvalidHeaderErrorString[] = "invalid header";
const StringRef HTTPHeader::ConstCoreHeader[]= {
#define ITEM(x) {(Byte *)(x), reinterpret_cast<size_t>(sizeof(x)-1)}
        ITEM("Host"),
        ITEM("Connection"),
        ITEM("If-Modified-Since"),
        ITEM("If-Unmodified-Since"),
        ITEM("If-Match"),
        ITEM("If-None-Match"),
        ITEM("UserAgent"),
        ITEM("Referer"),
        ITEM("Content-Length"),
        ITEM("Content-Range"),
        ITEM("Content-Type"),
        ITEM("Range"),
        ITEM("If-Range"),
        ITEM("Transfer-Encoding"),
        ITEM("TE"),
        ITEM("Expect"),
        ITEM("Upgrade"),
        ITEM("Accept-Encoding"),
        ITEM("Via"),
        ITEM("Authorization"),
        ITEM("Keep-Alive"),
        ITEM("X-Forward-For"),
        ITEM("X-Real-IP"),
        ITEM("Accept"),
        ITEM("Accept-Language"),
        ITEM("Depth"),
        ITEM("Destination"),
        ITEM("Overwrite"),
        ITEM("Date"),
        ITEM("Cookie"),
#undef ITEM
};
SmallVector<std::pair<uint32_t, StringRef>, 64> HTTPHeader::CoreHeaderMap = {};
StringRef& HTTPHeader::replaceCoreHeader(StringRef &origin) {
    if (CoreHeaderMap.empty()) {
        for(StringRef h : ConstCoreHeader) {
            CoreHeaderMap.push_back(std::pair<uint32_t, StringRef>(murmurhash2(h, true), h));
        }
    }
    uint32_t hash=murmurhash2(origin, true);
    for (auto &it : CoreHeaderMap)
        if(it.first==hash) return it.second;
    return origin;
}
HTTPError HTTPHeader::parse(MemoryBuffer &B, size_t Size, size_t &Off, bool Underscore) {
    enum HTTPHeaderParseState {
        HDR_START = 0,
        HDR_NAME,
        HDR_SPACE_BEFORE_VALUE,
        HDR_VALUE,
        HDR_SPACE_AFTER_VALUE,
        HDR_IGNORE_LINE,
        HDR_ALMOST_DONE,
        HDR_HEADER_ALMOST_DONE
    } State = HDR_START;
    char C1;
    auto it=B.begin()+Off, itEnd=std::min(B.end(), B.begin()+Size);
    auto HeaderStart=it, HeaderEnd=it;
    auto ValueStart=it, ValueEnd=it;
    for(; it!=itEnd && *it; it++) {
        switch (State) {
            case HDR_START:
                HeaderStart=ValueStart=it;
                switch (*it) {
                    case CR:
                        ValueEnd=it;
                        State = HDR_HEADER_ALMOST_DONE;
                        break;
                    case LF:
                        ValueEnd=it;
                        goto done;
                    default:
                        State = HDR_NAME;
                        HeaderStart = it;
                        C1 = lower_case[*it];
                        if (C1) break;
                        if (*it=='_'&&Underscore) break;
                        return {EINVAL, InvalidHeaderErrorString};
                }
                break;
            case HDR_NAME:
                C1 = lower_case[*it];
                if (C1) break;
                if (*it=='_') {
                    if (Underscore) break;
                    return {EINVAL, InvalidHeaderErrorString};
                }
                if (*it==':') {
                    HeaderEnd = it;
                    State = HDR_SPACE_BEFORE_VALUE;
                    break;
                }
                if (*it==CR) {
                    HeaderEnd = ValueStart = ValueEnd = it;
                    State = HDR_ALMOST_DONE;
                    break;
                }
                if (*it==LF) {
                    HeaderEnd = ValueStart = ValueEnd = it;
                    goto done;
                }
                if (*it=='/') {
                    if (!strncmp(reinterpret_cast<const char*>(HeaderStart), "HTTP", 4)) {
                        State = HDR_IGNORE_LINE;
                        break;
                    }
                }
                return {EINVAL, InvalidHeaderErrorString};
            case HDR_SPACE_BEFORE_VALUE:
                switch (*it) {
                    case ' ': break;
                    case CR:
                        ValueStart = ValueEnd = it;
                        State = HDR_ALMOST_DONE;
                        break;
                    case LF:
                        ValueStart = ValueEnd = it;
                        goto done;
                    case '\0':
                        return {EINVAL, InvalidHeaderErrorString};
                    default:
                        ValueStart=it;
                        State = HDR_VALUE;
                        break;
                }
                break;
            case HDR_VALUE:
                switch (*it) {
                    case ' ':
                        ValueEnd=it;
                        State = HDR_SPACE_AFTER_VALUE;
                        break;
                    case CR:
                        ValueEnd=it;
                        State = HDR_ALMOST_DONE;
                        break;
                    case LF:
                        ValueEnd=it;
                        goto done;
                    case '\0':
                        return {EINVAL, InvalidHeaderErrorString};
                }
                break;
            case HDR_SPACE_AFTER_VALUE:
                switch (*it) {
                    case ' ': break;
                    case CR:
                        State = HDR_ALMOST_DONE;
                        break;
                    case LF:
                        goto done;
                    case '\0':
                        return {EINVAL, InvalidHeaderErrorString};
                    default:
                        State = HDR_VALUE;
                        break;
                }
                break;
            case HDR_IGNORE_LINE:
                switch (*it) {
                    case LF:
                        State = HDR_START;
                        break;
                    default:
                        break;
                }
                break;
            case HDR_ALMOST_DONE:
                switch (*it) {
                    case LF:
                        goto done;
                    case CR:
                        break;
                    default:
                        return {EINVAL, InvalidHeaderErrorString};
                }
                break;
            case HDR_HEADER_ALMOST_DONE:
                switch (*it) {
                    case LF:
                        Off+=2,Header=Value={nullptr, 0};
                        return {ENOENT, NoMoreHeaderErrorString};
                    default:
                        return {EINVAL, InvalidHeaderErrorString};
                }
        }
    }
    return {EAGAIN, BrokenHeaderErrorString};
    done:
    if (HeaderEnd <= HeaderStart)
        return {EINVAL, InvalidHeaderErrorString};
    Header={const_cast<Byte *>(HeaderStart), (size_t)(HeaderEnd-HeaderStart)};
    if (ValueEnd <= ValueStart)
        return {EINVAL, InvalidHeaderErrorString};
    Header=replaceCoreHeader(Header);
    Value={const_cast<Byte *>(ValueStart), (size_t)(ValueEnd-ValueStart)};
    Off = (size_t)(it+1-B.begin());
    return {0};
}
size_t HTTPHeader::write(BufferWriter &W) {
    size_t write_size=0;
    if (Header.size()>0 && Value.size()>0) {
        write_size+=W.fromString(reinterpret_cast<const char*>(Header.begin()), Header.size());
        write_size+=W.fromString(": ", sizeof(": ")-1);
        write_size+=W.fromString(reinterpret_cast<const char*>(Value.begin()), Value.size());
        write_size+=W.fromString("\r\n", sizeof("\r\n")-1);
    }
    return write_size;
}

