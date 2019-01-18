#include "HTTP/HTTP.h"

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
        {"Host",                HI_HOST,              HeaderInFillVariable},
        {"Connection",          HI_CONNECTION,        HeaderInFillVariable},
        {"If-Modified-Since",   HI_IF_MODIFY_SINCE,   HeaderInFillVariable},
        {"If-Unmodified-Since", HI_IF_UNMODIFY_SINCE, HeaderInFillVariable},
        {"If-Match",            HI_IF_MATCH,          HeaderInFillVariable},
        {"If-None-Match",       HI_IF_NON_MATCH,      HeaderInFillVariable},
        {"UserAgent",           HI_USERAGENT,         HeaderInFillVariable},
        {"Referer",             HI_REFERENCE,         HeaderInFillVariable},
        {"Content-Length",      HI_CONTENT_LENGTH,    HeaderInFillVariable},
        {"Content-Range",       HI_CONTENT_RANGE,     HeaderInFillVariable},
        {"Content-Type",        HI_CONTENT_TYPE,      HeaderInFillVariable},
        {"Range",               HI_RANGE,             HeaderInFillVariable},
        {"If-Range",            HI_IF_RANGE,          HeaderInFillVariable},
        {"Transfer-Encoding",   HI_TRANSFER_ENCODING, HeaderInFillVariable},
        {"TE",                  HI_TE,                HeaderInFillVariable},
        {"Expect",              HI_EXPECT,            HeaderInFillVariable},
        {"Upgrade",             HI_UPGRADE,           HeaderInFillVariable},
        {"Accept-Encoding",     HI_ACCEPT_ENCODING,   HeaderInFillVariable},
        {"Via",                 HI_VIA,               HeaderInFillVariable},
        {"Authorization",       HI_AUTHORIZATION,     HeaderInFillVariable},
        {"Keep-Alive",          HI_KEEPALIVE,         HeaderInFillVariable},
        {"X-Forward-For",       HI_X_FORWARD_FOR,     HeaderInFillVariable},
        {"X-Real-IP",           HI_X_REAL_IP,         HeaderInFillVariable},
        {"Accept",              HI_ACCEPT,            HeaderInFillVariable},
        {"Accept-Language",     HI_ACCEPT_LANGUAGE,   HeaderInFillVariable},
        {"Depth",               HI_DEPTH,             HeaderInFillVariable},
        {"Destination",         HI_DESTINATION,       HeaderInFillVariable},
        {"Overwrite",           HI_OVERWRITE,         HeaderInFillVariable},
        {"Date",                HI_DATE,              HeaderInFillVariable},
        {"Cookie",              HI_COOKIE,            HeaderInFillVariable},
        {nullptr,               HI_COMMON,            nullptr},
};

static Dictionary HeaderInDictionary;

HTTPError HTTPRequest::ParseMethod(Buffer &B, HTTPRequest &R) {

    u_char C[11] = {0};
    BoundCursor BC;

    B >> BC;

    for (u_char C; C = *BC, !!BC; BC++) {

        if (C == CR || C == LF) {
            continue;
        }

        if ((C < 'A' || C > 'Z') && C != '_' && C != '-') {
            return {EFAULT, InvalidMethodErrorString};
        }

        break;
    }


    size_t ReadSize = BC.ReadBytes(C, 11);

    if (ReadSize < 5) {

        return {EAGAIN, BrokenRequestErrorString};
    } else if (C[3] == ' ') {

        if (C[0] == 'G' && C[1] == 'E' && C[2] == 'T') {
            R.Method = GET;
        } else if (C[0] == 'P' && C[1] == 'U' && C[2] == 'T') {
            R.Method = PUT;
        } else {
            return {EINVAL, BadMethodErrorString};
        }

        BC += 4;
    } else if (ReadSize > 5 && C[4] == ' ') {
        if (C[1] == 'O') {
            if (C[0] == 'P' && C[2] == 'S' && C[3] == 'T') {
                R.Method = POST;
            } else if (C[0] == 'C' && C[2] == 'P' && C[3] == 'Y') {
                R.Method = COPY;
            } else if (C[0] == 'M' && C[2] == 'V' && C[3] == 'E') {
                R.Method = MOVE;
            } else if (C[0] == 'L' && C[2] == 'C' && C[3] == 'K') {
                R.Method = LOCK;
            }
        } else if (C[0] == 'H' && C[1] == 'E' && C[2] == 'A' && C[3] == 'D') {
            R.Method = HEAD;
        } else {
            return {EFAULT, InvalidMethodErrorString};
        }

        BC += 5;
    } else if (ReadSize > 6 && C[5] == ' ') {

        if (C[0] == 'M' && C[1] == 'K' && C[2] == 'C' && C[3] == 'O' && C[4] == 'L') {
            R.Method = MKCOL;
        } else if (C[0] == 'P' && C[1] == 'A' && C[2] == 'T' && C[3] == 'C' && C[4] == 'H') {
            R.Method = PATCH;
        } else if (C[0] == 'T' && C[1] == 'R' && C[2] == 'A' && C[3] == 'C' && C[4] == 'E') {
            R.Method = TRACE;
        } else {
            return {EFAULT, InvalidMethodErrorString};
        }

        BC += 6;
    } else if (ReadSize > 7 && C[6] == ' ') {

        if (C[0] == 'D' && C[1] == 'E' && C[2] == 'L' && C[3] == 'E' && C[4] == 'T' && C[5] == 'E') {
            R.Method = DELETE;
        } else if (C[0] == 'U' && C[1] == 'N' && C[2] == 'L' && C[3] == 'O' && C[4] == 'C' && C[5] == 'K') {
            R.Method = UNLOCK;
        } else {
            return {EFAULT, InvalidMethodErrorString};
        }

        BC += 7;
    } else if (ReadSize > 8 && C[7] == ' ' && strncmp((char *) C, "OPTIONS", 7) == 0) {
        R.Method = OPTIONS;
    } else if (ReadSize > 9 && C[8] == ' ' && strncmp((char *) C, "PROPFIND", 8) == 0) {
        R.Method = PROPFIND;
    } else if (ReadSize > 10 && C[9] == ' ' && strncmp((char *) C, "PROPPATCH", 9) == 0) {
        R.Method = PROPPATCH;
    } else {
        return {EFAULT, InvalidMethodErrorString};
    }

    B << BC;

    return {0};
}

HTTPError HTTPRequest::ParseRequestLine(Buffer &B, HTTPRequest &R) {

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

    u_char C, C1;
    u_short HTTPMajor = 0, HTTPMinor = 0;
    BoundCursor BC, LastBC;

    for (B >> BC, LastBC = BC; C = *BC, !!BC; LastBC = BC++) {

        switch (RequestLineState) {

            case RL_SPACEBEFOREURI:

                if (C == '/') {
                    R.URI < BC;
                    RequestLineState = RL_AFTERSLASHINURI;
                    break;
                }

                C1 = (u_char) (C | 0x20);

                if (C1 >= 'a' && C1 <= 'z') {
                    R.Schema < BC;
                    RequestLineState = RL_SCHEMA;
                    break;
                }

                switch (C) {
                    case ' ':
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }
                break;

            case RL_SCHEMA:

                C1 = (u_char) (C | 0x20);

                if (C1 >= 'a' && C1 <= 'z') {
                    break;
                }

                if ((C >= '0' && C <= '9') || C == '+' || C == '-' || C == '.') {
                    break;
                }

                switch (C) {
                    case ':':
                        R.Schema > BC;
                        RequestLineState = RL_SCHEMASLASH;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;

            case RL_SCHEMASLASH:

                switch (C) {
                    case '/':
                        RequestLineState = RL_SCHEMASLASHSLASH;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;

            case RL_SCHEMASLASHSLASH:

                switch (C) {
                    case '/':
                        RequestLineState = RL_HOSTSTART;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;
            case RL_HOSTSTART:

                R.Host < BC;

                if (C == '[') {
                    RequestLineState = RL_HOSTIPLITERIAL;
                    break;
                }

                RequestLineState = RL_HOST;

            case RL_HOST:

                C1 = (u_char) (C | 0x20);

                if (C1 >= 'a' && C1 <= 'z') {
                    break;
                }

                if ((C1 >= '0' && C1 <= '0') || C1 == '.' || C1 == '-') {
                    break;
                }

            case RL_HOSTEND:

                R.Host > BC;

                switch (C) {
                    case ':':
                        RequestLineState = RL_PORT;
                        break;
                    case '/':
                        R.URI < BC;
                        RequestLineState = RL_AFTERSLASHINURI;
                        break;
                    case ' ':
                        R.URI < BC + 1, R.URI > BC + 2;
                        RequestLineState = RL_CHECKURIHTTP09;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;

            case RL_HOSTIPLITERIAL:

                if (C >= '0' && C <= '9') {
                    break;
                }

                C1 = (u_char) (C | 0x20);

                if (C1 >= 'a' && C1 <= 'z') {
                    break;
                }

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

                if (C >= '0' && C <= '9') {
                    break;
                }

                switch (C) {

                    case '/':
                        R.Port < BC, R.URI < BC;
                        RequestLineState = RL_AFTERSLASHINURI;
                        break;
                    case ' ':
                        R.Port = BC;
                        R.URI < BC + 1, R.URI > BC + 2;
                        RequestLineState = RL_HOSTHTTP09;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;

            case RL_HOSTHTTP09:

                switch (C) {
                    case ' ':
                        break;
                    case CR:
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        HTTPMinor = 9;
                        goto done;
                    case 'H':
                        R.HTTPProtocol = BC;
                        RequestLineState = RL_HTTP_H;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;

            case RL_AFTERSLASHINURI:

                if (usual[C >> 5] & (1U << (C & 0x1f))) {
                    RequestLineState = RL_CHECKURI;
                    break;
                }

                switch (C) {
                    case ' ':
                        R.URI > BC;
                        RequestLineState = RL_CHECKURIHTTP09;
                        break;
                    case CR:
                        R.URI > BC;
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        R.URI > BC;
                        HTTPMinor = 9;
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
                        R.Argument = BC + 1;
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

            case RL_CHECKURI:

                if (usual[C >> 5] & (1U << (C & 0x1f))) {
                    break;
                }

                switch (C) {
                    case '/':
#if 0
                        if (false) {
//                            if (r->uri_ext == p) {
//                                r->complex_uri = 1;
//                                RequestLineState = RL_URI;
//                                break;
//                            }
                        }
//                        r->uri_ext = NULL;
#endif
                        RequestLineState = RL_AFTERSLASHINURI;
                        break;
                    case '.':
//                        r->uri_ext = p + 1;
                        break;
                    case ' ':
                        R.URI > BC;
                        RequestLineState = RL_CHECKURIHTTP09;
                        break;
                    case CR:
                        R.URI > BC;
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        R.URI > BC;
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
                        R.QuotedURI = 1;
                        RequestLineState = RL_URI;
                        break;
                    case '?':
                        R.Argument = BC + 1;
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
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;

            case RL_CHECKURIHTTP09:

                switch (C) {
                    case ' ':
                        break;
                    case CR:
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        HTTPMinor = 9;
                        goto done;
                    case 'H':
                        R.HTTPProtocol = BC;
                        RequestLineState = RL_HTTP_H;
                        break;
                    default:
                        R.SpaceInURI = 1;
                        RequestLineState = RL_CHECKURI;
                        BC = LastBC;
                        break;
                }

                break;

            case RL_URI:

                if (usual[C >> 5] & (1U << (C & 0x1f))) {
                    break;
                }

                switch (C) {
                    case ' ':
                        R.URI > BC;
                        RequestLineState = RL_HTTP09;
                        break;
                    case CR:
                        R.URI > BC;
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        R.URI > BC;
                        goto done;
                    case '#':
                        R.ComplexURI = 1;
                        break;
                    case '\0':
                        return {EFAULT, BadRequestErrorString};
                }

                break;

                /* space+ after URI */
            case RL_HTTP09:
                switch (C) {
                    case ' ':
                        break;
                    case CR:
                        HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        HTTPMinor = 9;
                        goto done;
                    case 'H':
                        R.HTTPProtocol = BC;
                        RequestLineState = RL_HTTP_H;
                        break;
                    default:
                        R.SpaceInURI = 1;
                        RequestLineState = RL_URI;
                        BC = LastBC;
                        break;
                }

                break;

            case RL_HTTP_H:

                switch (C) {
                    case 'T':
                        RequestLineState = RL_HTTP_HT;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;

            case RL_HTTP_HT:

                switch (C) {
                    case 'T':
                        RequestLineState = RL_HTTP_HTT;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;

            case RL_HTTP_HTT:

                switch (C) {
                    case 'P':
                        RequestLineState = RL_HTTP_HTTP;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;

            case RL_HTTP_HTTP:

                switch (C) {
                    case '/':
                        RequestLineState = RL_FIRSTMAJORDIGIT;
                        break;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;

            case RL_FIRSTMAJORDIGIT:

                if (C < '1' || C > '9') {
                    return {EFAULT, BadRequestErrorString};
                }

                HTTPMajor = C - '0';

                if (HTTPMajor > 1) {
                    return {EFAULT, BadVersionErrorString};
                }

                RequestLineState = RL_MAJORDIGIT;

                break;

            case RL_MAJORDIGIT:

                if (C == '.') {
                    RequestLineState = RL_FIRSTMINORDIGIT;
                    break;
                }

                if (C < '0' || C > '9') {
                    return {EFAULT, BadRequestErrorString};
                }

                HTTPMajor = HTTPMajor * 10 + (C - '0');

                if (HTTPMajor > 1) {
                    return {EFAULT, BadVersionErrorString};
                }

                break;

            case RL_FIRSTMINORDIGIT:

                if (C < '0' || C > '9') {
                    return {EFAULT, BadRequestErrorString};
                }

                HTTPMinor = C - '0';
                RequestLineState = RL_MINORDIGIT;

                break;

            case RL_MINORDIGIT:

                if (C == CR) {
                    RequestLineState = RL_ALMOSTDONE;
                    break;
                }

                if (C == LF) {
                    goto done;
                }

                if (C == ' ') {
                    RequestLineState = RL_SPACEAFTERDIGIT;
                    break;
                }

                if (C < '0' || C > '9') {
                    return {EFAULT, BadRequestErrorString};
                }

                if (HTTPMinor > 99) {
                    return {EFAULT, BadRequestErrorString};
                }

                HTTPMinor = HTTPMinor * 10 + (C - '0');

                break;

            case RL_SPACEAFTERDIGIT:

                switch (C) {
                    case ' ':
                        break;
                    case CR:
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        goto done;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;

            case RL_ALMOSTDONE:

                switch (C) {
                    case LF:
                        goto done;
                    default:
                        return {EFAULT, BadRequestErrorString};
                }

                break;
        }
    }

    return {EAGAIN, BrokenRequestErrorString};

    done:

    if (!(BC + 1)) {
        return {EAGAIN, BrokenRequestErrorString};
    }

    BC += 1, B << BC;

    R.Version = HTTPMajor * 1000 + HTTPMinor;

    if (R.Version == 9 && R.Method != GET) {
        return {EFAULT, BadVersionErrorString};
    }

    return {0};
}


HTTPError HTTPRequest::ParseRequestHeaders(Buffer &B, HTTPRequest &R, bool AllowUnderScore) {

    HTTPError Error{0};
    HTTPHeader Header;

    // init header process if not
    if (HeaderInDictionary.Begin() == nullptr) {

        int i = 0;

        while (HeaderInProcesses[i].GetKey() != nullptr) {
            HeaderInDictionary.Insert(HeaderInProcesses[i]), i++;
        }
    }

    uint32_t Hash;

    do {

        Error = Header.Read(B, AllowUnderScore);

        if (Error.GetCode() != 0) {
            if (Error.GetCode() == ENOENT) {
                break;
            } else {
                return Error;
            }
        }

        Hash = 0 ^ Header.Name.Size();

        for (BoundCursor BC = Header.Name; *BC != '\0'; BC++) {
            SimpleHash(Hash, lower_case[*BC]);
        }

        DictionaryItem *TempPrev, *TempNext, *DI = HeaderInDictionary.Find(Hash);

        if (DI != nullptr) {

            TempPrev = (DictionaryItem *) HeaderInDictionary.Prev(DI);
            TempNext = (DictionaryItem *) HeaderInDictionary.Next(DI);

            if ((TempPrev != nullptr && TempPrev->GetHash() == DI->GetHash()) ||
                (TempNext != nullptr && TempNext->GetHash() == DI->GetHash())) {
                // [TODO] Collision found, should add warning here
            }

            Error = ((HTTPCoreHeader *) DI)->Process(R, Header);

            if (Error.GetCode() != 0) {
                return Error;
            }

        } else {

            HTTPHeader *H = R.HeaderIn.Headers.Push();

            if (H == nullptr) {
                return {ENOMEM, NoMemoryErrorString};
            }

            *H = Header;
        }

    } while (true);

    return Error;
}

HTTPError HTTPRequest::ParseRequestURI(Buffer *B, HTTPRequest &R) {

    enum URIParseState {
        URI_START = 0,
        URI_AFTER_SLASH_IN_URI,
        URI_CHECK_URI,
        URI_URI
    };

    BoundCursor BC = R.URI;
    URIParseState State = URI_START;

    for (u_char C, C1; C = *BC, !!BC; BC++) {

        switch (State) {
            case URI_START:

                if (C != '/') {
                    return {EINVAL, BadURIErrorString};
                }

                State = URI_AFTER_SLASH_IN_URI;

                break;

            case URI_AFTER_SLASH_IN_URI:

                if (usual[C >> 5] & (1U << (C & 0x1F))) {
                    State = URI_CHECK_URI;

                    break;
                }

                switch (C) {
                    case ' ':
                        R.SpaceInURI = 1;
                        State = URI_CHECK_URI;
                        break;
                    case '.':
                        R.ComplexURI = 1;
                        State = URI_URI;
                        break;
                    case '%':
                        R.QuotedURI = 1;
                        State = URI_URI;
                        break;
                    case '/':
                        R.ComplexURI = 1;
                        State = URI_URI;
                        break;
#if 0
                    case '\\':
                        R.ComplexURI = 1;
                        State = URI_URI;
                        break;
#endif
                    case '?':
                        R.Argument = BC + 1;
                        State = URI_URI;
                        break;
                    case '#':
                        R.ComplexURI = 1;
                        State = URI_URI;
                        break;
                    case '+':
                        R.PlusInURI = 1;
                        break;
                    default:
                        State = URI_CHECK_URI;
                        break;
                }

                break;

            case URI_CHECK_URI:

                if (usual[C >> 5] & (1U << (C & 0x1F))) {
                    break;
                }

                switch (C) {
                    case '/':
#if 0
                        if ( R.URIExt == BC) {
                            R.ComplexURI = 1;
                            State = URI_URI;
                            break;
                        }
#endif
                        R.URIExt.Position = nullptr;
                        State = URI_AFTER_SLASH_IN_URI;
                        break;
                    case '.':
                        R.URIExt = BC + 1;
                        break;
                    case ' ':
                        R.SpaceInURI = 1;
                        break;
#if 0
                    case '\\':
                        R.ComplexURI = 1;
                        State = URI_AFTER_SLASH_IN_URI;
                        break;
#endif
                    case '%':
                        R.QuotedURI = 1;
                        State = URI_URI;
                        break;
                    case '?':
                        R.Argument = BC + 1;
                        State = URI_URI;
                        break;
                    case '#':
                        R.ComplexURI = 1;
                        State = URI_URI;
                        break;
                    case '+':
                        R.PlusInURI = 1;
                        break;
                }

                break;

            case URI_URI:

                if (usual[C >> 5] & (1U << (C << 0x1F))) {
                    break;
                }

                switch (C) {
                    case ' ':
                        R.SpaceInURI = 1;
                        break;
                    case '#':
                        R.ComplexURI = 1;
                        break;
                }

                break;
        }
    }

    return {0};
}

HTTPError HTTPRequest::HeaderInFillVariable(HTTPCoreHeader &C, HTTPRequest &R, HTTPHeader &H) {

    switch (C.GetType()) {
        case HI_HOST:
            R.HeaderIn.Host = H.Value;
            break;
        case HI_CONNECTION:
            R.HeaderIn.Connection = H.Value;
            break;
        case HI_IF_MODIFY_SINCE:
            R.HeaderIn.IfModufiedSince = H.Value;
            break;
        case HI_IF_UNMODIFY_SINCE:
            R.HeaderIn.IfUnModifiedSince = H.Value;
            break;
        case HI_IF_MATCH:
            R.HeaderIn.IfMatch = H.Value;
            break;
        case HI_IF_NON_MATCH:
            R.HeaderIn.IfNonMatch = H.Value;
            break;
        case HI_USERAGENT:
            R.HeaderIn.UserAgent = H.Value;
            // [TODO]support browser detection
            break;
        case HI_ACCEPT_ENCODING:
            R.HeaderIn.AcceptEncoding = H.Value;
            break;
        case HI_REFERENCE:
            R.HeaderIn.Referer = H.Value;
            break;
        case HI_CONTENT_LENGTH:
            R.HeaderIn.ContentLength = H.Value;
            // [TODO] parse content size
            break;
        case HI_CONTENT_RANGE:
            R.HeaderIn.ContentRange = H.Value;
            break;
        case HI_CONTENT_TYPE:
            R.HeaderIn.ContentType = H.Value;
            break;
        case HI_RANGE:
            R.HeaderIn.Range = H.Value;
            break;
        case HI_IF_RANGE:
            R.HeaderIn.IfRange = H.Value;
            break;
        case HI_TRANSFER_ENCODING:
            R.HeaderIn.TransferEncoding = H.Value;
            break;
        case HI_TE:
            R.HeaderIn.TE = H.Value;
            break;
        default:
            return {EINVAL, BadCoreHeaderInErrorString};
    }

    return {0};
}

HTTPError HTTPRequest::Read(Buffer &B) {

    HTTPError Error(0);

    switch (State) {

        case HTTP_INIT:
            State = HTTP_PAESE_METHOD;
        case HTTP_PAESE_METHOD:

            Error = ParseMethod(B, *this);

            if (Error.GetCode() == 0) {
                State = HTTP_PARSE_REQUEST_LINE;
            } else {
                State = HTTP_BAD_REQUEST;
                break;
            }

        case HTTP_PARSE_REQUEST_LINE:

            Error = ParseRequestLine(B, *this);

            if (Error.GetCode() == 0) {
                State = HTTP_PARSE_HEADER;
            } else {
                State = HTTP_BAD_REQUEST;

                break;
            }

        case HTTP_PARSE_HEADER:

            do {
                Error = ParseRequestHeaders(B, *this);
            } while (Error.GetCode() == 0 && Error.CodeMessage() == nullptr);

            if (Error.GetCode() == 0) {

                State = HTTP_HEADER_DONE;
            } else {
                State = HTTP_BAD_REQUEST;

                break;
            }

        case HTTP_HEADER_DONE:
            // to parse done or read chunk
            break;
        case HTTP_BAD_REQUEST:
        default:
            Error = {EINVAL, BadRequestErrorString};
            break;
    }

    return Error;

}

void HTTPRequest::Reset() {
    State = HTTP_INIT, Context = nullptr;
}

