#include <cerrno>
#include "HTTP/HTTPError.h"
#include "HTTP/HTTPHeader.h"

using namespace std;
using namespace ngx::HTTP;
const char BrokenHeaderErrorString[] = "Broken Header in buffer";
const char NoMoreHeaderErrorString[] = "No more header";
const char InvalidHeaderErrorString[] = "Invalid Header";
HTTPError HTTPHeader::parse(ngx::Core::Support::MemoryBuffer &B, size_t &Off, bool Underscore) {
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
    auto it=B.begin()+Off;
    auto HeaderStart=it, HeaderEnd=it;
    auto ValueStart=it, ValueEnd=it;
    for(; it!=B.end(); it++) {
        switch (State) {
            case HDR_START:
                HeaderStart=ValueStart=it;
                switch (*it) {
                    case CR:
                        ValueEnd=it;
                        State = HDR_HEADER_ALMOST_DONE;
                        return {ENOENT, NoMoreHeaderErrorString};
                        break;
                    case LF:
                        ValueEnd=it;
                        goto done;
                    default:
                        State = HDR_NAME;
                        C1 = lower_case[*it];
                        if (C1) break;
                        if (*it=='_') {
                            if (Underscore) break;
                        }
                        return {EINVAL, InvalidHeaderErrorString};
                }
                break;
            case HDR_NAME:
                C1 = lower_case[*it];
                if (C1) {
                    break;
                }
                if (*it=='_') {
                    if (Underscore) {
                        break;
                    }
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
                    if (strncmp(reinterpret_cast<const char*>(HeaderStart), "HTTP", 4)) {
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
    Header={const_cast<Byte *>(ValueStart), (size_t)(ValueEnd-ValueStart)};
    Off = (size_t)(it-B.begin());
    return {0};
}



