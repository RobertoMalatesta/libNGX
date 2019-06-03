#include "HTTP/HTTPError.h"
#include "HTTP/HTTPHeader.h"
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPCoreHeader.h"

using namespace ngx::HTTP;

const char BadURIErrorString[] = "Bad URI";
const char BadMethodErrorString[] = "Bad Method";
const char BadRequestErrorString[] = "Bad Request";
const char BadVersionErrorString[] = "Bad HTTP Version";
const char InvalidMethodErrorString[] = "Invalid Method";
const char BrokenRequestErrorString[] = "Broken Request in buffer";
const char NoMemoryErrorString[] = "No sufficient memory to store header indexer";
const char BadCoreHeaderInErrorString[] = "Bad core header in";

HTTPCoreHeader HTTPRequest::HeaderInProcesses[] = {
        {"Host",                HI_HOST,              nullptr},
        {"Connection",          HI_CONNECTION,        nullptr},
        {"If-Modified-Since",   HI_IF_MODIFY_SINCE,   nullptr},
        {"If-Unmodified-Since", HI_IF_UNMODIFY_SINCE, nullptr},
        {"If-Match",            HI_IF_MATCH,          nullptr},
        {"If-None-Match",       HI_IF_NON_MATCH,      nullptr},
        {"UserAgent",           HI_USERAGENT,         nullptr},
        {"Referer",             HI_REFERENCE,         nullptr},
        {"Content-Length",      HI_CONTENT_LENGTH,    nullptr},
        {"Content-Range",       HI_CONTENT_RANGE,     nullptr},
        {"Content-Type",        HI_CONTENT_TYPE,      nullptr},
        {"Range",               HI_RANGE,             nullptr},
        {"If-Range",            HI_IF_RANGE,          nullptr},
        {"Transfer-Encoding",   HI_TRANSFER_ENCODING, nullptr},
        {"TE",                  HI_TE,                nullptr},
        {"Expect",              HI_EXPECT,            nullptr},
        {"Upgrade",             HI_UPGRADE,           nullptr},
        {"Accept-Encoding",     HI_ACCEPT_ENCODING,   nullptr},
        {"Via",                 HI_VIA,               nullptr},
        {"Authorization",       HI_AUTHORIZATION,     nullptr},
        {"Keep-Alive",          HI_KEEPALIVE,         nullptr},
        {"X-Forward-For",       HI_X_FORWARD_FOR,     nullptr},
        {"X-Real-IP",           HI_X_REAL_IP,         nullptr},
        {"Accept",              HI_ACCEPT,            nullptr},
        {"Accept-Language",     HI_ACCEPT_LANGUAGE,   nullptr},
        {"Depth",               HI_DEPTH,             nullptr},
        {"Destination",         HI_DESTINATION,       nullptr},
        {"Overwrite",           HI_OVERWRITE,         nullptr},
        {"Date",                HI_DATE,              nullptr},
        {"Cookie",              HI_COOKIE,            nullptr},
        {nullptr,               HI_COMMON,            nullptr},
};

static Dictionary HeaderInDictionary;

HTTPError HTTPRequest::ParseMethod(WritableMemoryBuffer &B, HTTPRequest &R) {
    for(auto C : B) {
        if(C==CR || C == LF) continue;
        if ((C<'A'||C>'Z') && C!='_' && C!='-')
            return {EFAULT, InvalidMethodErrorString};
        break;
    }
    auto it = B.begin();
    if (B.size() < 5)
        return {EAGAIN, BrokenRequestErrorString};
    else if (it[3]==' ') {
        if (it[0] == 'G' && it[1] == 'E' && it[2] == 'T') R.Method=GET;
        else if (it[0] == 'P' && it[1] == 'U' && it[2] == 'T') R.Method=PUT;
        else return {EINVAL, BadMethodErrorString};
        R.MethodStr={it, 3};
    } else if (B.size()>5 && it[4]==' ') {
        if (it[1] == 'O') {
            if (it[0]=='P' && it[2]=='S' && it[3]=='T') R.Method=POST;
            else if (it[0]=='C' && it[2]=='P' && it[3]=='Y') R.Method=COPY;
            else if (it[0]=='M' && it[2]=='V' && it[3]=='E') R.Method=MOVE;
            else if (it[0]=='L' && it[2]=='C' && it[3] == 'K') R.Method=LOCK;
        }
        else if (it[0]=='H' && it[1]=='E' && it[2]=='A' && it[3]=='D') R.Method=HEAD;
        else return {EFAULT, InvalidMethodErrorString};
        R.MethodStr={it, 4};
    } else if (B.size()>6 && it[5]==' ') {
        if (it[0]=='M' && it[1]=='K' && it[2]=='C' && it[3]=='O' && it[4]=='L') R.Method=MKCOL;
        else if (it[0]=='P' && it[1]=='A' && it[2]=='T' && it[3]=='C' && it[4]=='H') R.Method=PATCH;
        else if (it[0]=='T' && it[1]=='R' && it[2]=='A' && it[3]=='C' && it[4]=='E') R.Method=TRACE;
        else return {EFAULT, InvalidMethodErrorString};
        R.MethodStr={it, 5};
    } else if (B.size()>7 && it[6]==' ') {
        if (it[0]=='D' && it[1]=='E' && it[2]=='L' && it[3]=='E' && it[4]=='T' && it[5]=='E') R.Method=DELETE;
        else if (it[0]=='U' && it[1]=='N' && it[2]=='L' && it[3]=='O' && it[4]=='C' && it[5]=='K') R.Method=UNLOCK;
        else return {EFAULT, InvalidMethodErrorString};
        R.MethodStr={it, 6};
    } else if (B.size()>8 && it[7]==' ' && strncmp(reinterpret_cast<char *>(it), "OPTIONS", 7) == 0) R.Method=OPTIONS, R.MethodStr={it, 7};
    else if (B.size()>9 && it[8]==' ' && strncmp(reinterpret_cast<char *>(it), "PROPFIND", 8) == 0) R.Method=PROPFIND, R.MethodStr={it, 8};
    else if (B.size()>10 && it[9]==' ' && strncmp(reinterpret_cast<char *>(it), "PROPPATCH", 9) == 0) R.Method=PROPFIND, R.MethodStr={it, 9};
    else return {EFAULT, InvalidMethodErrorString};
    return {0};
}

HTTPError HTTPRequest::ParseRequestLine(WritableMemoryBuffer &B, HTTPRequest &R) {
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
    if (R.MethodStr.size()<3) return {EINVAL, InvalidMethodErrorString};
    u_char C1;
    u_short HTTPMajor = 0, HTTPMinor = 0;
    auto it=B.begin()+R.MethodStr.size();
    auto ArgumentStart=it;
    auto RequestLineStart=B.begin(), RequestLineEnd=it;
    auto SchemeStart=it, SchemeEnd=it;
    auto HostStart=it, HostEnd=it;
    auto PortStart=it, PortEnd=it;
    auto HTTPStart=it, HTTPEnd=it;
    auto URIStart=it, URIEnd=it;
    for(; it!=B.end(); it++) {
        switch (RequestLineState) {
            case RL_SPACEBEFOREURI:
                if (*it=='/') {
                    URIStart=it;
                    RequestLineState=RL_AFTERSLASHINURI;
                    break;
                }
                C1=reinterpret_cast<u_char >(*it)|0x20;
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
                C1=reinterpret_cast<u_char >(*it)|0x20;
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
                C1=reinterpret_cast<u_char >(*it)|0x20;
                if (C1 >= 'a' && C1 <= 'z') break;
                if ((C1 >= '0' && C1 <= '0') || C1 == '.' || C1 == '-') break;
            case RL_HOSTEND:
                if (it<HostStart) return {EFAULT, BadRequestErrorString};
                HostEnd=it;
                switch (*it) {
                    case ':':
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
                C1=reinterpret_cast<u_char >(*it)|0x20;
                if (C1>='a' && C1<='z') break;
                switch (C1) {
                    case ':':
                    case ']':
                        RequestLineState = RL_HOSTEND;
                        break;
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
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_PORT:
                if (*it>='0' && *it<='9') break;
                switch (*it) {
                    case '/':
                        PortStart=URIStart=it;
                        RequestLineState = RL_AFTERSLASHINURI;
                        break;
                    case ' ':
                        R.Port={PortStart, (size_t)(it-PortStart)};
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
                        RequestLineEnd=it;
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
                        URIEnd=it;
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        URIEnd=it;
                        HTTPMinor = 9;
                        RequestLineEnd=it;
                        goto done;
                    case '.':
                        R.ComplexURI = 1;
                        RequestLineState = RL_URI;
                        break;
                    case '%':
                        R.QuotedURI = 1;
                        RequestLineState = RL_URI;
                        break;
                    case '/':
                        R.ComplexURI = 1;
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
                        R.ComplexURI = 1;
                        RequestLineState = RL_URI;
                        break;
                    case '+':
                        R.PlusInURI = 1;
                        break;
                    case '\0':
                        return {EFAULT, BadRequestErrorString};
                    default:
                        RequestLineState = RL_CHECKURI;
                        break;
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
                        RequestLineEnd=it;
                        goto done;
                    case 'H':
                        HTTPStart=it;
                        RequestLineState = RL_HTTP_H;
                        break;
                    default:
                        R.SpaceInURI = 1;
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
                        RequestLineEnd=it;
                        goto done;
                    case '#':
                        R.ComplexURI = 1;
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
                        RequestLineEnd=it;
                        goto done;
                    case 'H':
                        HTTPStart=it;
                        RequestLineState = RL_HTTP_H;
                        break;
                    default:
                        R.SpaceInURI = 1;
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
                HTTPEnd=it+1;
                break;
            case RL_MINORDIGIT:
                if (*it==CR) {
                    RequestLineState = RL_ALMOSTDONE;
                    break;
                }
                if (*it==LF) {
                    RequestLineEnd=it;
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
                        RequestLineEnd=it;
                        goto done;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
            case RL_ALMOSTDONE:
                switch (*it) {
                    case LF:
                        RequestLineEnd=it;
                        goto done;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;
        }
    }
    return {EAGAIN, BrokenRequestErrorString};
done:
    if(RequestLineEnd==B.end())
        return {EAGAIN, BrokenRequestErrorString};
    if(RequestLineEnd<RequestLineStart)
        return {EFAULT, BadRequestErrorString};
    R.RequestLine={RequestLineStart, (size_t)(RequestLineEnd-RequestLineStart)};
    if(SchemeEnd>=SchemeStart)
        R.Schema={SchemeStart, (size_t)(SchemeEnd-SchemeStart)};
    if (HostEnd>=HostStart)
        R.Host={HostStart, (size_t)(HostEnd-HostStart)};
    if (PortEnd>=PortStart)
        R.Port={PortStart, (size_t)(PortEnd-PortStart)};
    if (HTTPEnd>=HTTPStart)
        R.HTTPProtocol={HTTPStart, (size_t)(HTTPEnd-HTTPStart)};
    if (URIEnd>=URIStart)
        R.URI={URIStart, (size_t)(URIEnd-URIStart)};
    if (URIEnd>=ArgumentStart && ArgumentStart>URIStart)
        R.Argument={ArgumentStart, (size_t)(URIEnd-ArgumentStart)};
    R.Version = HTTPMajor * 1000 + HTTPMinor;
    if (R.Version == 9 && R.Method != GET)
        return {EFAULT, BadVersionErrorString};
    return {0};
}

//HTTPError HTTPRequest::ParseRequestHeaders(Buffer &B, HTTPRequest &R, bool AllowUnderScore) {
//    HTTPError Error{0};
//    HTTPHeader Header;
//    // init header process if not
//    if (HeaderInDictionary.begin() == nullptr) {
//        int i = 0;
//        while (HeaderInProcesses[i].GetKey() != nullptr) {
//            HeaderInDictionary.Insert(HeaderInProcesses[i]), i++;
//        }
//    }
//    uint32_t Hash;
//    do {
//        Error = Header.Read(B, AllowUnderScore);
//        if (Error.GetCode() != 0) {
//            if (Error.GetCode() == ENOENT) {
//                break;
//            } else {
//                return Error;
//            }
//        }
//        Hash = 0 ^ Header.Name.Size();
//        for (BoundCursor BC = Header.Name; *BC != '\0'; BC++) {
//            SimpleHash(Hash, lower_case[*BC]);
//        }
//        DictionaryItem *TempPrev, *TempNext, *DI = HeaderInDictionary.Find(Hash);
//        if (DI != nullptr) {
//            TempPrev = (DictionaryItem *) HeaderInDictionary.prev(DI);
//            TempNext = (DictionaryItem *) HeaderInDictionary.next(DI);
//            if ((TempPrev != nullptr && TempPrev->GetHash() == DI->GetHash()) ||
//                (TempNext != nullptr && TempNext->GetHash() == DI->GetHash())) {
//                // [TODO] Collision found, should add warning here
//            }
//            Error = ((HTTPCoreHeader *) DI)->Process(R, Header);
//            if (Error.GetCode() != 0) {
//                return Error;
//            }
//        } else {
////            HTTPHeader *H = R.HeaderIn.Headers.Push();
////            if (H == nullptr) {
////                return {ENOMEM, NoMemoryErrorString};
////            }
////            *H = Header;
//        }
//    } while (true);
//    return Error;
//}
//
//HTTPError HTTPRequest::ParseRequestURI(Buffer *B, HTTPRequest &R) {
//    enum URIParseState {
//        URI_START = 0,
//        URI_AFTER_SLASH_IN_URI,
//        URI_CHECK_URI,
//        URI_URI
//    };
//    BoundCursor BC = R.URI;
//    URIParseState State = URI_START;
//    for (u_char C, C1; C = *BC, !!BC; BC++) {
//        switch (State) {
//            case URI_START:
//                if (C != '/') {
//                    return {EINVAL, BadURIErrorString};
//                }
//                State = URI_AFTER_SLASH_IN_URI;
//                break;
//            case URI_AFTER_SLASH_IN_URI:
//                if (usual[C >> 5] & (1U << (C & 0x1F))) {
//                    State = URI_CHECK_URI;
//                    break;
//                }
//                switch (C) {
//                    case ' ':
//                        R.SpaceInURI = 1;
//                        State = URI_CHECK_URI;
//                        break;
//                    case '.':
//                        R.ComplexURI = 1;
//                        State = URI_URI;
//                        break;
//                    case '%':
//                        R.QuotedURI = 1;
//                        State = URI_URI;
//                        break;
//                    case '/':
//                        R.ComplexURI = 1;
//                        State = URI_URI;
//                        break;
//#if 0
//                    case '\\':
//                        R.ComplexURI = 1;
//                        State = URI_URI;
//                        break;
//#endif
//                    case '?':
//                        R.Argument = BC + 1;
//                        State = URI_URI;
//                        break;
//                    case '#':
//                        R.ComplexURI = 1;
//                        State = URI_URI;
//                        break;
//                    case '+':
//                        R.PlusInURI = 1;
//                        break;
//                    default:
//                        State = URI_CHECK_URI;
//                        break;
//                }
//                break;
//            case URI_CHECK_URI:
//                if (usual[C >> 5] & (1U << (C & 0x1F))) {
//                    break;
//                }
//                switch (C) {
//                    case '/':
//#if 0
//                        if ( R.URIExt == BC) {
//                            R.ComplexURI = 1;
//                            State = URI_URI;
//                            break;
//                        }
//#endif
//                        R.URIExt.Position = nullptr;
//                        State = URI_AFTER_SLASH_IN_URI;
//                        break;
//                    case '.':
//                        R.URIExt = BC + 1;
//                        break;
//                    case ' ':
//                        R.SpaceInURI = 1;
//                        break;
//#if 0
//                    case '\\':
//                        R.ComplexURI = 1;
//                        State = URI_AFTER_SLASH_IN_URI;
//                        break;
//#endif
//                    case '%':
//                        R.QuotedURI = 1;
//                        State = URI_URI;
//                        break;
//                    case '?':
//                        R.Argument = BC + 1;
//                        State = URI_URI;
//                        break;
//                    case '#':
//                        R.ComplexURI = 1;
//                        State = URI_URI;
//                        break;
//                    case '+':
//                        R.PlusInURI = 1;
//                        break;
//                }
//                break;
//            case URI_URI:
//                if (usual[C >> 5] & (1U << (C << 0x1F))) {
//                    break;
//                }
//                switch (C) {
//                    case ' ':
//                        R.SpaceInURI = 1;
//                        break;
//                    case '#':
//                        R.ComplexURI = 1;
//                        break;
//                }
//                break;
//        }
//    }
//    return {0};
//}
//
//HTTPError HTTPRequest::HeaderInFillVariable(HTTPCoreHeader &C, HTTPRequest &R, HTTPHeader &H) {
//    switch (C.GetType()) {
//        case HI_HOST:
//            R.HeaderIn.Host = H.Value;
//            break;
//        case HI_CONNECTION:
//            R.HeaderIn.Connection = H.Value;
//            break;
//        case HI_IF_MODIFY_SINCE:
//            R.HeaderIn.IfModufiedSince = H.Value;
//            break;
//        case HI_IF_UNMODIFY_SINCE:
//            R.HeaderIn.IfUnModifiedSince = H.Value;
//            break;
//        case HI_IF_MATCH:
//            R.HeaderIn.IfMatch = H.Value;
//            break;
//        case HI_IF_NON_MATCH:
//            R.HeaderIn.IfNonMatch = H.Value;
//            break;
//        case HI_USERAGENT:
//            R.HeaderIn.UserAgent = H.Value;
//            // [TODO]support browser detection
//            break;
//        case HI_ACCEPT_ENCODING:
//            R.HeaderIn.AcceptEncoding = H.Value;
//            break;
//        case HI_REFERENCE:
//            R.HeaderIn.Referer = H.Value;
//            break;
//        case HI_CONTENT_LENGTH:
//            R.HeaderIn.ContentLength = H.Value;
//            // [TODO] parse content size
//            break;
//        case HI_CONTENT_RANGE:
//            R.HeaderIn.ContentRange = H.Value;
//            break;
//        case HI_CONTENT_TYPE:
//            R.HeaderIn.ContentType = H.Value;
//            break;
//        case HI_RANGE:
//            R.HeaderIn.Range = H.Value;
//            break;
//        case HI_IF_RANGE:
//            R.HeaderIn.IfRange = H.Value;
//            break;
//        case HI_TRANSFER_ENCODING:
//            R.HeaderIn.TransferEncoding = H.Value;
//            break;
//        case HI_TE:
//            R.HeaderIn.TE = H.Value;
//            break;
//        default:
//            return {EINVAL, BadCoreHeaderInErrorString};
//    }
//    return {0};
//}
//
//HTTPError HTTPRequest::Read(Buffer &B) {
//    HTTPError Error(0);
//    switch (State) {
//        case HTTP_INIT:
//            State = HTTP_PAESE_METHOD;
//        case HTTP_PAESE_METHOD:
//
//            Error = ParseMethod(B, *this);
//
//            if (Error.GetCode() == 0) {
//                State = HTTP_PARSE_REQUEST_LINE;
//            } else {
//                State = HTTP_BAD_REQUEST;
//                break;
//            }
//
//        case HTTP_PARSE_REQUEST_LINE:
//
//            Error = ParseRequestLine(B, *this);
//
//            if (Error.GetCode() == 0) {
//                State = HTTP_PARSE_HEADER;
//            } else {
//                State = HTTP_BAD_REQUEST;
//
//                break;
//            }
//
//        case HTTP_PARSE_HEADER:
//
//            do {
//                Error = ParseRequestHeaders(B, *this);
//            } while (Error.GetCode() == 0 && Error.CodeMessage() == nullptr);
//
//            if (Error.GetCode() == 0) {
//
//                State = HTTP_HEADER_DONE;
//            } else {
//                State = HTTP_BAD_REQUEST;
//
//                break;
//            }
//
//        case HTTP_HEADER_DONE:
//            // to parse done or read chunk
//            break;
//        case HTTP_BAD_REQUEST:
//        default:
//            Error = {EINVAL, BadRequestErrorString};
//            break;
//    }
//
//    return Error;
//
//}
//
//void HTTPRequest::Reset() {
//    State = HTTP_INIT, Context = nullptr;
//}

