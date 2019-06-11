#include "HTTP/HTTPError.h"
#include "HTTP/HTTPHeader.h"
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPCoreHeader.h"
using namespace ngx::HTTP;
const char BadURIErrorString[] = "bad URI";
const char BadMethodErrorString[] = "bad method";
const char BadRequestErrorString[] = "bad request";
const char BadVersionErrorString[] = "bad HTTP version";
const char InvalidMethodErrorString[] = "invalid method";
const char BrokenRequestErrorString[] = "broken request in buffer";
const char NoMemoryErrorString[] = "no sufficient memory to store header indexer";
const char BadHeaderErrorString[] = "bad header error";
void HTTPRequest::processCoreHeader(HTTPHeader &H, uint32_t hash) {
}
HTTPError HTTPRequest::parseMethod(const StringRef &TopHalf) {
    auto it=TopHalf.char_begin();
    for(;it!=TopHalf.char_end() && (*it); it++) {
        if(*it==CR || *it == LF) continue;
        if ((*it<'A'|| *it>'Z') && *it!='_' && *it!='-')
            return {EFAULT, InvalidMethodErrorString};
        break;
    }
    if ((TopHalf.char_end()-it) < 5)
        return {EAGAIN, BrokenRequestErrorString};
    else if (it[3]==' ') {
        if (it[0] == 'G' && it[1] == 'E' && it[2] == 'T') Method=GET;
        else if (it[0] == 'P' && it[1] == 'U' && it[2] == 'T') Method=PUT;
        else return {EINVAL, BadMethodErrorString};
        MethodStr={reinterpret_cast<const Byte *>(it), 3};
    } else if (TopHalf.size()>5 && it[4]==' ') {
        if (it[1] == 'O') {
            if (it[0]=='P' && it[2]=='S' && it[3]=='T') Method=POST;
            else if (it[0]=='C' && it[2]=='P' && it[3]=='Y') Method=COPY;
            else if (it[0]=='M' && it[2]=='V' && it[3]=='E') Method=MOVE;
            else if (it[0]=='L' && it[2]=='C' && it[3] == 'K') Method=LOCK;
        }
        else if (it[0]=='H' && it[1]=='E' && it[2]=='A' && it[3]=='D') Method=HEAD;
        else return {EFAULT, InvalidMethodErrorString};
        MethodStr={reinterpret_cast<const Byte *>(it), 4};
    } else if (TopHalf.size()>6 && it[5]==' ') {
        if (it[0]=='M' && it[1]=='K' && it[2]=='C' && it[3]=='O' && it[4]=='L') Method=MKCOL;
        else if (it[0]=='P' && it[1]=='A' && it[2]=='T' && it[3]=='C' && it[4]=='H') Method=PATCH;
        else if (it[0]=='T' && it[1]=='R' && it[2]=='A' && it[3]=='C' && it[4]=='E') Method=TRACE;
        else return {EFAULT, InvalidMethodErrorString};
        MethodStr={reinterpret_cast<const Byte *>(it), 5};
    } else if (TopHalf.size()>7 && it[6]==' ') {
        if (it[0]=='D' && it[1]=='E' && it[2]=='L' && it[3]=='E' && it[4]=='T' && it[5]=='E') Method=DELETE;
        else if (it[0]=='U' && it[1]=='N' && it[2]=='L' && it[3]=='O' && it[4]=='C' && it[5]=='K') Method=UNLOCK;
        else return {EFAULT, InvalidMethodErrorString};
        MethodStr={reinterpret_cast<const Byte *>(it), 6};
    } else if (TopHalf.size()>8 && it[7]==' ' && strncmp(it, "OPTIONS", 7) == 0)
        Method=OPTIONS, MethodStr={reinterpret_cast<const Byte *>(it), 7};
    else if (TopHalf.size()>9 && it[8]==' ' && strncmp(it, "PROPFIND", 8) == 0)
        Method=PROPFIND, MethodStr={reinterpret_cast<const Byte *>(it), 8};
    else if (TopHalf.size()>10 && it[9]==' ' && strncmp(it, "PROPPATCH", 9) == 0)
        Method=PROPPATCH, MethodStr={reinterpret_cast<const Byte *>(it), 9};
    else return {EFAULT, InvalidMethodErrorString};
    return {0};
}
HTTPError HTTPRequest::parseRequestline(const StringRef &TopHalf) {
    enum HTTPRequestLineParseState {
        RL_SPACEBEFOREURI = 0,
        RL_SCHEMA,
        RL_SCHEMASLASH,
        RL_SCHEMASLASHSLASH,
        RL_HOSTSTART,
        RL_HOST,
        RL_HOSTEND,
        RL_HOSTIPLITERIAL,
        RL_PORT,
        RL_HOSTHTTP09,
        RL_AFTERSLASHINURI,
        RL_CHECKURI,
        RL_CHECKURIHTTP09,
        RL_URI,
        RL_HTTP09,
        RL_HTTP_H,
        RL_HTTP_HT,
        RL_HTTP_HTT,
        RL_HTTP_HTTP,
        RL_FIRSTMAJORDIGIT,
        RL_MAJORDIGIT,
        RL_FIRSTMINORDIGIT,
        RL_MINORDIGIT,
        RL_SPACEAFTERDIGIT,
        RL_ALMOSTDONE
    } RequestLineState = RL_SPACEBEFOREURI;
    if (MethodStr.size()<3) return {EINVAL, InvalidMethodErrorString};
    char C1;
    u_short HTTPMajor = 0, HTTPMinor = 0;
    auto it=TopHalf.char_begin()+MethodStr.size();
    auto ArgumentStart=it;
    auto RequestLineStart=TopHalf.char_begin(), RequestLineEnd=it;
    auto SchemeStart=it, SchemeEnd=it;
    auto HostStart=it, HostEnd=it;
    auto PortStart=it, PortEnd=it;
    auto HTTPStart=it, HTTPEnd=it;
    auto URIStart=it, URIEnd=it;
    for(; it!=TopHalf.char_end() && *it; it++) {
        switch (RequestLineState) {
            case RL_SPACEBEFOREURI:
                if (*it=='/') {
                    URIStart=it;
                    RequestLineState=RL_AFTERSLASHINURI;
                    break;
                }
                C1=(*it)|0x20;
                if (C1>='a' && C1<='z') {
                    SchemeStart=it;
                    RequestLineState=RL_SCHEMA;
                    break;
                }
                switch (C1) {
                    case ' ':
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_SCHEMA:
                C1=(*it)|0x20;
                if (C1 >= 'a' && C1 <= 'z') break;
                if ((*it>='0' && *it<='9') || *it=='+'||*it=='-'||*it=='.') break;
                switch (*it) {
                    case ':':
                        SchemeEnd=it;
                        RequestLineState = RL_SCHEMASLASH;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_SCHEMASLASH:
                switch (*it) {
                    case '/':
                        RequestLineState = RL_SCHEMASLASHSLASH;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_SCHEMASLASHSLASH:
                switch (*it) {
                    case '/':
                        RequestLineState = RL_HOSTSTART;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_HOSTSTART:
                HostStart=it;
                if (*it=='[') {
                    RequestLineState = RL_HOSTIPLITERIAL;
                    break;
                }
                RequestLineState = RL_HOST;
            case RL_HOST:
                C1=(*it)|0x20;
                if (C1 >= 'a' && C1 <= 'z') break;
                if ((C1 >= '0' && C1 <= '9') || C1 == '.' || C1 == '-') break;
            case RL_HOSTEND:
                if (it<HostStart) return {EFAULT, BadRequestErrorString};
                HostEnd=it;
                switch (*it) {
                    case ':':
                        PortStart=it+1;
                        RequestLineState = RL_PORT;
                        break;
                    case '/':
                        URIStart=it;
                        RequestLineState = RL_AFTERSLASHINURI;
                        break;
                    case ' ':
                        URIStart=it+1, URIEnd=it+2;
                        RequestLineState = RL_CHECKURIHTTP09;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_HOSTIPLITERIAL:
                if (*it>='0' && *it <= '9') break;
                C1=(*it)|0x20;
                if (C1>='a' && C1<='z') break;
                switch (*it) {
                    case ']':
                        HostEnd=it+1;
                        RequestLineState = RL_HOSTEND;
                        break;
                    case ':':
                    case '-':
                    case '.':
                    case '_':
                    case '~':
                        break;
                    case '!':
                    case '$':
                    case '&':
                    case '\'':
                    case '(':
                    case ')':
                    case '*':
                    case '+':
                    case ',':
                    case ';':
                    case '=':
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_PORT:
                if (*it>='0' && *it<='9') break;
                switch (*it) {
                    case '/':
                        PortEnd=URIStart=it;
                        RequestLineState = RL_AFTERSLASHINURI;
                        break;
                    case ' ':
                        PortEnd=it;
                        URIStart=it+1, URIEnd=it+2;
                        RequestLineState = RL_HOSTHTTP09;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_HOSTHTTP09:
                switch (*it) {
                    case ' ':
                        break;
                    case CR:
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        HTTPMinor = 9;
                        RequestLineEnd=it+1;
                        goto done;
                    case 'H':
                        HTTPStart=it;
                        RequestLineState = RL_HTTP_H;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_AFTERSLASHINURI:
                if (usual[*it>>5] & (1U<<(*it&0x1f))) {
                    RequestLineState = RL_CHECKURI;
                    break;
                }
                switch (*it) {
                    case ' ':
                        URIEnd=it;
                        RequestLineState = RL_CHECKURIHTTP09;
                        break;
                    case CR:
                        URIEnd=it-1;
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        URIEnd=it-2;
                        HTTPMinor = 9;
                        RequestLineEnd=it+1;
                        goto done;
                    case '.':
                        ComplexURI = 1;
                        RequestLineState = RL_URI;
                        break;
                    case '%':
                        QuotedURI = 1;
                        RequestLineState = RL_URI;
                        break;
                    case '/':
                        ComplexURI = 1;
                        RequestLineState = RL_URI;
                        break;
                    case '\\':
#if 0
                        if (false) {
                           ComplexURI = 1;
                           RequestLineState = RL_URI;
                        }
#endif
                        break;
                    case '?':
                        ArgumentStart=it+1;
                        RequestLineState = RL_URI;
                        break;
                    case '#':
                        ComplexURI = 1;
                        RequestLineState = RL_URI;
                        break;
                    case '+':
                        PlusInURI = 1;
                        break;
                    case '\0':
                        return {EFAULT, BadRequestErrorString};
                    default:
                        RequestLineState = RL_CHECKURI;
                        break;
                }
                break;
            case RL_CHECKURI:
                if (usual[*it>>5]&(1U<<(*it&0x1F))) break;
                switch (*it) {
                    case '/':
                        RequestLineState = RL_AFTERSLASHINURI;
                        break;
                    case '.':
                        break;
                    case ' ':
                        URIEnd = it;
                        RequestLineState = RL_CHECKURIHTTP09;
                        break;
                    case CR:
                        URIEnd = it;
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        URIEnd = it;
                        HTTPMinor = 9;
                        goto done;
                    case '\\':
#if 0
                        if (false) {
                            ComplexURI = 1;
                            RequestLineState = RL_AfterSlashInURI;
                        }
#endif
                        break;
                    case '%':
                        QuotedURI = 1;
                        RequestLineState = RL_URI;
                        break;
                    case '?':
                        ArgumentStart = it+1;
                        RequestLineState = RL_URI;
                        break;
                    case '#':
                        ComplexURI = 1;
                        RequestLineState = RL_URI;
                        break;
                    case '+':
                        PlusInURI = 1;
                        break;
                    case '\0':
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_CHECKURIHTTP09:
                switch (*it) {
                    case ' ':
                        break;
                    case CR:
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        HTTPMinor = 9;
                        RequestLineEnd=it+1;
                        goto done;
                    case 'H':
                        HTTPStart=it;
                        RequestLineState = RL_HTTP_H;
                        break;
                    default:
                        SpaceInURI = 1;
                        RequestLineState = RL_CHECKURI;
                        it-=1;
                        break;
                }
                break;
            case RL_URI:
                if (usual[*it>>5] & (1U<<(*it&0x1f))) break;
                switch (*it) {
                    case ' ':
                        URIEnd=it;
                        RequestLineState = RL_HTTP09;
                        break;
                    case CR:
                        URIEnd=it;
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        URIEnd=it;
                        RequestLineEnd=it+1;
                        goto done;
                    case '#':
                        ComplexURI = 1;
                        break;
                    case '?':
                        ArgumentStart=it+1;
                        break;
                    case '\0':
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_HTTP09:
                switch (*it) {
                    case ' ':
                        break;
                    case CR:
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        HTTPMinor = 9;
                        RequestLineEnd=it+1;
                        goto done;
                    case 'H':
                        HTTPStart=it;
                        RequestLineState = RL_HTTP_H;
                        break;
                    default:
                        SpaceInURI = 1;
                        RequestLineState = RL_URI;
                        it--;
                        break;
                }
                break;
            case RL_HTTP_H:
                switch (*it) {
                    case 'T':
                        RequestLineState = RL_HTTP_HT;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_HTTP_HT:
                switch (*it) {
                    case 'T':
                        RequestLineState = RL_HTTP_HTT;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_HTTP_HTT:
                switch (*it) {
                    case 'P':
                        RequestLineState = RL_HTTP_HTTP;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_HTTP_HTTP:
                switch (*it) {
                    case '/':
                        RequestLineState = RL_FIRSTMAJORDIGIT;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_FIRSTMAJORDIGIT:
                if (*it<'1' || *it>'9')
                    return {EFAULT, BadRequestErrorString};
                HTTPMajor = *it - '0';
                if (HTTPMajor > 1) {
                    return {EFAULT, BadVersionErrorString};
                }
                RequestLineState = RL_MAJORDIGIT;
                break;
            case RL_MAJORDIGIT:
                if (*it=='.') {
                    RequestLineState = RL_FIRSTMINORDIGIT;
                    break;
                }
                if (*it<'0' || *it>'9')
                    return {EFAULT, BadRequestErrorString};
                HTTPMajor = HTTPMajor * 10 + (*it - '0');
                if (HTTPMajor > 1)
                    return {EFAULT, BadVersionErrorString};
                break;
            case RL_FIRSTMINORDIGIT:
                if (*it<'0' || *it>'9')
                    return {EFAULT, BadRequestErrorString};
                HTTPMinor = *it-'0';
                RequestLineState = RL_MINORDIGIT;
                break;
            case RL_MINORDIGIT:
                if (*it==CR) {
                    HTTPEnd=it;
                    RequestLineState = RL_ALMOSTDONE;
                    break;
                }
                if (*it==LF) {
                    HTTPEnd=it-1;
                    RequestLineEnd=it+1;
                    goto done;
                }
                if (*it==' ') {
                    RequestLineState = RL_SPACEAFTERDIGIT;
                    break;
                }
                if (*it<'0' || *it > '9')
                    return {EFAULT, BadRequestErrorString};
                if (HTTPMinor > 99)
                    return {EFAULT, BadRequestErrorString};
                HTTPMinor = HTTPMinor * 10 + (*it - '0');
                HTTPEnd=it+1;
                break;
            case RL_SPACEAFTERDIGIT:
                switch (*it) {
                    case ' ':
                        break;
                    case CR:
                        RequestLineState=RL_ALMOSTDONE;
                        break;
                    case LF:
                        RequestLineEnd=it+1;
                        goto done;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_ALMOSTDONE:
                switch (*it) {
                    case LF:
                        RequestLineEnd=it+1;
                        goto done;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
        }
    }
    return {EAGAIN, BrokenRequestErrorString};
done:
    if(RequestLineEnd>=TopHalf.char_end())
        return {EAGAIN, BrokenRequestErrorString};
    if(RequestLineEnd<RequestLineStart)
        return {EFAULT, BadRequestErrorString};
    RequestLine={reinterpret_cast<const Byte*>(RequestLineStart), (size_t)(RequestLineEnd-RequestLineStart)};
    if(SchemeEnd>=SchemeStart)
        Scheme={reinterpret_cast<const Byte*>(SchemeStart), (size_t)(SchemeEnd-SchemeStart)};
    if (HostEnd>=HostStart)
        Host={reinterpret_cast<const Byte*>(HostStart), (size_t)(HostEnd-HostStart)};
    if (PortEnd>=PortStart)
        Port={reinterpret_cast<const Byte*>(PortStart), (size_t)(PortEnd-PortStart)};
    if (HTTPEnd>=HTTPStart)
        Protocol={reinterpret_cast<const Byte*>(HTTPStart), (size_t)(HTTPEnd-HTTPStart)};
    if (URIEnd>=URIStart)
        URI={reinterpret_cast<const Byte*>(URIStart), (size_t)(URIEnd-URIStart)};
    if (URIEnd>=ArgumentStart && ArgumentStart>URIStart)
        Argument={reinterpret_cast<const Byte*>(ArgumentStart), (size_t)(URIEnd-ArgumentStart)};
    Version = HTTPMajor * 1000 + HTTPMinor;
    if (Version == 9 && Method != GET)
        return {EFAULT, BadVersionErrorString};
    return {0};
}
HTTPError HTTPRequest::parseRequestURI(const StringRef &TopHalf) {
    enum URIParseState {
        URI_START = 0,
        URI_AFTER_SLASH_IN_URI,
        URI_CHECK_URI,
        URI_URI
    } State=URI_START;
    auto I=TopHalf.begin();
    auto URIStart=I, ArgumentStart=I;
    for (; I!=TopHalf.end() && *I; I++) {
        switch (State) {
            case URI_START:
                if (*I != '/') return {EINVAL, BadURIErrorString};
                State = URI_AFTER_SLASH_IN_URI;
                break;
            case URI_AFTER_SLASH_IN_URI:
                if (usual[*I>>5] & (1U << (*I&0x1F))) {
                    State = URI_CHECK_URI;
                    break;
                }
                switch (*I) {
                    case ' ':
                        SpaceInURI = 1;
                        State = URI_CHECK_URI;
                        break;
                    case '.':
                        ComplexURI = 1;
                        State = URI_URI;
                        break;
                    case '%':
                        QuotedURI = 1;
                        State = URI_URI;
                        break;
                    case '/':
                        ComplexURI = 1;
                        State = URI_URI;
                        break;
#if 0
                    case '\\':
                        R.ComplexURI = 1;
                        State = URI_URI;
                        break;
#endif
                    case '?':
                        ArgumentStart=(I)+1;
                        State = URI_URI;
                        break;
                    case '#':
                        ComplexURI = 1;
                        State = URI_URI;
                        break;
                    case '+':
                        PlusInURI = 1;
                        break;
                    default:
                        State = URI_CHECK_URI;
                        break;
                }
                break;
            case URI_CHECK_URI:
                if (usual[*I>>5] & (1U<<(*I&0x1F))) break;
                switch (*I) {
                    case '/':
#if 0
                        if ( R.URIExt == BC) {
                            R.ComplexURI=1;
                            State=URI_URI;
                            break;
                        }
#endif
                        State = URI_AFTER_SLASH_IN_URI;
                        break;
                    case '.':
                        break;
                    case ' ':
                        SpaceInURI = 1;
                        break;
#if 0
                    case '\\':
                        R.ComplexURI = 1;
                        State = URI_AFTER_SLASH_IN_URI;
                        break;
#endif
                    case '%':
                        QuotedURI = 1;
                        State = URI_URI;
                        break;
                    case '?':
                        ArgumentStart=I+1;
                        State = URI_URI;
                        break;
                    case '#':
                        ComplexURI = 1;
                        State = URI_URI;
                        break;
                    case '+':
                        PlusInURI = 1;
                        break;
                }
                break;
            case URI_URI:
                if (usual[*I>>5] & (1U<<(*I<<0x1F))) break;
                switch (*I) {
                    case ' ':
                        SpaceInURI=1;
                        break;
                    case '#':
                        ComplexURI=1;
                        break;
                }
                break;
        }
    }
    if (URIStart>TopHalf.begin())
        URI={const_cast<const Byte *>(URIStart), (size_t)(I-URIStart)};
    if (ArgumentStart>TopHalf.begin())
        URI={const_cast<const Byte *>(ArgumentStart), (size_t)(I-ArgumentStart)};
    return {0};
}
HTTPError HTTPRequest::parseHeaders(const StringRef &TopHalf) {
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
HTTPError HTTPRequest::parse(const StringRef &TopHalf, const StringRef &BottomHalf) {
    HTTPError err(0);
    const Byte *BodyTHStart;
    switch (State) {
        case HTTPRequestState::HTTP_INIT:
            BodyTH=BodyBH={}, HeadersOffset=0;
            State=HTTPRequestState::HTTP_PAESE_METHOD;
        case HTTPRequestState::HTTP_PAESE_METHOD:
            err=parseMethod(TopHalf);
            if (err.GetCode()!=0)
                return err;
            State=HTTPRequestState::HTTP_PARSE_REQUEST_LINE;
        case HTTPRequestState::HTTP_PARSE_REQUEST_LINE:
            err=parseRequestline(TopHalf);
            if (err.GetCode()!=0)
                return err;
            HeadersOffset = RequestLine.size();
            State=HTTPRequestState::HTTP_PARSE_HEADER;
        case HTTPRequestState::HTTP_PARSE_HEADER:
            err=parseHeaders(TopHalf);
            BodyTHStart=TopHalf.begin()+HeadersOffset;
            if (err.GetCode()!=0 || BodyTHStart>=TopHalf.end())
                return err;
            BodyTH={BodyTHStart, (size_t)(BodyTH.end()-BodyTHStart)};
            State=HTTPRequestState::HTTP_HEADER_DONE;
        case HTTPRequestState::HTTP_HEADER_DONE:
            BodyTHStart=BodyTH.begin();
            BodyTH={BodyTHStart, (size_t)(BodyTH.end()-BodyTHStart)};
            BodyBH=BottomHalf;
            return {0};
    }
    HeaderMap.reset_to_small();
    return {EBADE, BadRequestErrorString};
}
HTTPError HTTPRequest::write(BufferWriter &W) {
    return {0};
}
//HTTPCoreHeader HTTPRequest::HeaderInProcesses[] = {
//        {"Host",                HI_HOST,              nullptr},
//        {"Connection",          HI_CONNECTION,        nullptr},
//        {"If-Modified-Since",   HI_IF_MODIFY_SINCE,   nullptr},
//        {"If-Unmodified-Since", HI_IF_UNMODIFY_SINCE, nullptr},
//        {"If-Match",            HI_IF_MATCH,          nullptr},
//        {"If-None-Match",       HI_IF_NON_MATCH,      nullptr},
//        {"UserAgent",           HI_USERAGENT,         nullptr},
//        {"Referer",             HI_REFERENCE,         nullptr},
//        {"Content-Length",      HI_CONTENT_LENGTH,    nullptr},
//        {"Content-Range",       HI_CONTENT_RANGE,     nullptr},
//        {"Content-Type",        HI_CONTENT_TYPE,      nullptr},
//        {"Range",               HI_RANGE,             nullptr},
//        {"If-Range",            HI_IF_RANGE,          nullptr},
//        {"Transfer-Encoding",   HI_TRANSFER_ENCODING, nullptr},
//        {"TE",                  HI_TE,                nullptr},
//        {"Expect",              HI_EXPECT,            nullptr},
//        {"Upgrade",             HI_UPGRADE,           nullptr},
//        {"Accept-Encoding",     HI_ACCEPT_ENCODING,   nullptr},
//        {"Via",                 HI_VIA,               nullptr},
//        {"Authorization",       HI_AUTHORIZATION,     nullptr},
//        {"Keep-Alive",          HI_KEEPALIVE,         nullptr},
//        {"X-Forward-For",       HI_X_FORWARD_FOR,     nullptr},
//        {"X-Real-IP",           HI_X_REAL_IP,         nullptr},
//        {"Accept",              HI_ACCEPT,            nullptr},
//        {"Accept-Language",     HI_ACCEPT_LANGUAGE,   nullptr},
//        {"Depth",               HI_DEPTH,             nullptr},
//        {"Destination",         HI_DESTINATION,       nullptr},
//        {"Overwrite",           HI_OVERWRITE,         nullptr},
//        {"Date",                HI_DATE,              nullptr},
//        {"Cookie",              HI_COOKIE,            nullptr},
//        {nullptr,               HI_COMMON,            nullptr},
//};


