#include "HTTP/HTTPError.h"
#include "HTTP/HTTPHeader.h"
#include "HTTP/HTTPCoreHeader.h"
#include "HTTP/HTTPResponse.h"
using namespace ngx::HTTP;
const char BadHeaderErrorString[] = "bad header error";
const char BadResponseLineErrorString[] = "bad response line error";
const char BrokenResponseLineErrorString[] = "broken response line error";
void HTTPResponse::processCoreHeader(HTTPHeader &H, uint32_t hash) {
}
HTTPError HTTPResponse::parseResponseline(const StringRef &TopHalf) {
    enum HTTPResponseLineParseStates {
        RL_START = 0,
        RL_H,
        RL_HT,
        RL_HTT,
        RL_HTTP,
        RL_FIRST_MAJOR_DIGIT,
        RL_MAJOR_DIGIT,
        RL_FIRST_MINOR_DIGIT,
        RL_MINOR_DIGIT,
        RL_STATUS,
        RL_SPACE_AFTER_STATUS,
        RL_STATUS_TEXT,
        RL_ALMOST_DONE,
    } ResponseLineState = RL_START;
    u_short HTTPMajor = 0, HTTPMinor = 0;
    auto it=TopHalf.char_begin();
    auto StatusCodeStart=it, StatusCodeEnd=it;
    auto StatusTextStart=it, StatusTextEnd=it;
    auto StatusLineStart=it, StatusLineEnd=it;
    for(;it!=TopHalf.char_end() && *it; it++) {
        switch (ResponseLineState) {
            case RL_START:
                switch (*it) {
                    case 'H':
                        ResponseLineState = RL_H;
                        break;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }
                break;
            case RL_H:
                switch (*it) {
                    case 'T':
                        ResponseLineState = RL_HT;
                        break;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }
                break;
            case RL_HT:
                switch (*it) {
                    case 'T':
                        ResponseLineState = RL_HTT;
                        break;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }
                break;
            case RL_HTT:
                switch (*it) {
                    case 'P':
                        ResponseLineState = RL_HTTP;
                        break;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }
                break;
            case RL_HTTP:
                switch (*it) {
                    case '/':
                        ResponseLineState = RL_FIRST_MAJOR_DIGIT;
                        break;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }
                break;
            case RL_FIRST_MAJOR_DIGIT:
                if (*it<'1' || *it>'9')
                    return {EINVAL, BadResponseLineErrorString};
                HTTPMajor = *it-'0', ResponseLineState = RL_MAJOR_DIGIT;
                break;
            case RL_MAJOR_DIGIT:
                if (*it=='.') {
                    ResponseLineState = RL_FIRST_MINOR_DIGIT;
                    break;
                }
                if (*it<'1' || *it>'9')
                    return {EINVAL, BadResponseLineErrorString};
                if (HTTPMajor>99)
                    return {EINVAL, BadResponseLineErrorString};
                HTTPMajor = 10 * HTTPMajor + (*it-'0');
                break;
            case RL_FIRST_MINOR_DIGIT:
                if (*it<'1' || *it>'9')
                    return {EINVAL, BadResponseLineErrorString};
                HTTPMinor = *it-'0', ResponseLineState = RL_MINOR_DIGIT;
                break;
            case RL_MINOR_DIGIT:
                if (*it==' ') {
                    StatusCodeStart=it+1;
                    ResponseLineState = RL_STATUS;
                    break;
                }
                if (*it<'1' || *it>'9')
                    return {EINVAL, BadResponseLineErrorString};
                if (HTTPMinor > 99)
                    return {EINVAL, BadResponseLineErrorString};
                HTTPMinor = 10 * HTTPMinor + (*it-'0');
                break;
            case RL_STATUS:
                if (*it==' ') {
                    ResponseLineState = RL_STATUS;
                    break;
                }
                if (*it<'0' || *it>'9')
                    return {EINVAL, BadResponseLineErrorString};
                StatusCode=StatusCode*10+(*it-'0');
                StatusCodeEnd=it+1;
                if(StatusCodeEnd-StatusCodeStart==3 || StatusCode>999) {
                    ResponseLineState = RL_SPACE_AFTER_STATUS;
                    break;
                }
                break;
            case RL_SPACE_AFTER_STATUS:
                switch (*it) {
                    case ' ':
                    case '.':
                        ResponseLineState = RL_STATUS_TEXT;
                        StatusTextStart=it+1;
                        break;
                    case CR:
                        ResponseLineState = RL_ALMOST_DONE;
                        break;
                    case LF:
                        StatusLineEnd=it;
                        goto done;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }
                break;
            case RL_STATUS_TEXT:
                switch (*it) {
                    case CR:
                        StatusTextEnd=it;
                        ResponseLineState = RL_ALMOST_DONE;
                        break;
                    case LF:
                        StatusCodeEnd=it-1;
                        StatusLineEnd=it;
                        goto done;
                }
                break;
            case RL_ALMOST_DONE:
                switch (*it) {
                    case LF:
                        StatusLineEnd=it;
                        goto done;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }
        }
    }
    return {EINVAL, BrokenResponseLineErrorString};
    done:
    if (it+1>=TopHalf.char_end())
        return {EAGAIN, BrokenResponseLineErrorString};
    Version=HTTPMajor*1000+HTTPMinor;
    if (StatusCodeEnd>StatusCodeStart)
        StatusCodeStr={reinterpret_cast<const Byte*>(StatusCodeStart), (size_t)(StatusCodeEnd-StatusCodeStart)};
    if (StatusTextEnd>StatusTextStart)
        StatusText={reinterpret_cast<const Byte*>(StatusTextStart), (size_t)(StatusTextEnd-StatusTextStart)};
    if (StatusLineEnd>StatusLineStart)
        StatusLine={reinterpret_cast<const Byte*>(StatusLineStart), (size_t)(StatusLineEnd-StatusLineStart+1)};
    return {0};
}
HTTPError HTTPResponse::parseHeaders(const StringRef &TopHalf) {
    HTTPHeader h;
    HTTPError e(0);
    if (HeadersOffset==0) return {EBADE, BadHeaderErrorString };
    while ((e=h.parse(TopHalf, HeadersOffset, ALLOW_UNDERSCORE)).GetCode()==0) {
        bool found=false;
        uint32_t hash=murmurhash2(h.getHeader(), true);
        for (auto &h1 : HeaderMap)
            if (h1.first==hash) {
                found=true, h1.second=h;
                processCoreHeader(h, hash);
                break;
            }
        if (!found) {
            HeaderMap.push_back(std::pair<uint32_t, HTTPHeader>(hash, h));
            processCoreHeader(h, hash);
        }
    }
    if (e.GetCode()==ENOENT) return {0};
    return e;
}
HTTPError HTTPResponse::parse(const StringRef &TopHalf, const StringRef &BottomHalf) {
    HTTPError err(0);
    const Byte *BodyTHStart;
    switch (State) {
        case HTTPResponseState ::HTTP_INIT:
            BodyTH=BodyBH={}, HeadersOffset=0;
            State=HTTPResponseState::HTTP_PARSE_RESPONSE_LINE;
        case HTTPResponseState::HTTP_PARSE_RESPONSE_LINE:
            err=parseResponseline(TopHalf);
            if (err.GetCode()!=0)
                return err;
            HeadersOffset = StatusLine.size();
            State=HTTPResponseState::HTTP_PARSE_HEADERS;
        case HTTPResponseState::HTTP_PARSE_HEADERS:
            err=parseHeaders(TopHalf);
            BodyTHStart=TopHalf.begin()+HeadersOffset;
            if (err.GetCode()!=0 || BodyTHStart>=TopHalf.end())
                return err;
            BodyTH={BodyTHStart, (size_t)(BodyTH.end()-BodyTHStart)};
            State=HTTPResponseState::HTTP_HEADER_DONE;
        case HTTPResponseState::HTTP_HEADER_DONE:
            BodyTHStart=BodyTH.begin();
            BodyTH={BodyTHStart, (size_t)(BodyTH.end()-BodyTHStart)};
            BodyBH=BottomHalf;
            return {0};
    }
    HeaderMap.reset_to_small();
    return {EBADE, BadResponseLineErrorString};
}
HTTPError HTTPResponse::write(BufferWriter &W) {
    return {0};
}
