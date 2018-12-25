#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

const char BadURIErrorString[] = "Bad URI";
const char BadRequestErrorString[] = "Bad Request";
const char BadVersionErrorString[] = "Bad HTTP Version";
const char InvalidMethodErrorString[] = "Invalid Method";
const char InvalidHeaderErrorString[] = "Invalid Header";
const char BrokenRequestErrorString[] = "Broken Request in buffer";
const char BrokenHeaderErrorString[] = "Broken Header in buffer";
const char NoMoreHeaderErrorString[] = "No more header";
const char NoMemoryErrorString[] = "No sufficient memory to store header indexer";
const char BadCoreHeaderInErrorString[] = "Bad core header in";

const uint32_t usual[] = {
        0xffffdbfe, /* 1111 1111 1111 1111  1101 1011 1111 1110 */
        /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x7fff37d6, /* 0111 1111 1111 1111  0011 0111 1101 0110 */
        /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
//#if (NGX_WIN32)
//        0xefffffff, /* 1110 1111 1111 1111  1111 1111 1111 1111 */
//#else
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
//#endif
        /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
};

const HTTPCoreHeader HeaderInProcesses[31] = {
        {"Host", HI_HOST, HTTPRequest::HeaderInFillVariable},
        {"Connection", HI_CONNECTION, nullptr},
        {"If-Modified-Since", HI_IF_MODIFY_SINCE, nullptr},
        {"If-Unmodified-Since", HI_IF_UNMODIFY_SINCE, nullptr},
        {"If-Match", HI_IF_MATCH, nullptr},
        {"If-None-Match", HI_IF_NON_MATCH, nullptr},
        {"UserAgent", HI_USERAGENT, nullptr},
        {"Referer", HI_REFERENCE, nullptr},
        {"Content-Length", HI_CONTENT_LENGTH, nullptr},
        {"Content-Range", HI_CONTENT_RANGE, nullptr},
        {"Content-Type", HI_CONTENT_TYPE, nullptr},
        {"Range", HI_RANGE, nullptr},
        {"If-Range", HI_IF_RANGE, nullptr},
        {"Transfer-Encoding", HI_TRANSFER_ENCODING, nullptr},
        {"TE", HI_TE, nullptr},
        {"Epect", HI_EXPECT, nullptr},
        {"Upgrade", HI_UPGRADE, nullptr},
        {"Accept-Encoding", HI_ACCEPT_ENCODING, HTTPRequest::HeaderInFillVariable},
        {"Via", HI_VIA, nullptr},
        {"Authorization", HI_AUTHORIZATION, nullptr},
        {"Keep-Alive", HI_KEEPALIVE, nullptr},
        {"X-Forward-For", HI_X_FORWARD_FOR, nullptr},
        {"X-Real-IP", HI_X_REAL_IP, nullptr},
        {"Accept", HI_ACCEPT, nullptr},
        {"Accept-Language", HI_ACCEPT_LANGUAGE, nullptr},
        {"Depth", HI_DEPTH, nullptr},
        {"Destinaion", HI_DESTINATION, nullptr},
        {"Overwrite", HI_OVERWRITE, nullptr},
        {"Date", HI_DATE, nullptr},
        {"Cookie", HI_COOKIE, nullptr},
        {"Size", HI_SIZE, nullptr},
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
            return {EINVAL, InvalidHeaderErrorString};
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
    } else if (ReadSize > 8 && C[7] == ' ' && strncmp((char *)C, "OPTIONS", 7) == 0) {
        R.Method = OPTIONS;
    } else if (ReadSize > 9 && C[8] == ' ' && strncmp((char *)C, "PROPFIND", 8) == 0) {
        R.Method = PROPFIND;
    } else if (ReadSize > 10 && C[9] == ' ' && strncmp((char *)C, "PROPPATCH", 9) == 0) {
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

HTTPError HTTPRequest::ParseHeader(Buffer &B, HTTPHeader &Header, bool AllowUnderScore) {

    enum HTTPHeaderParseState {
        HDR_START = 0,
        HDR_NAME,
        HDR_SPACE_BEFORE_VALUE,
        HDR_VALUE,
        HDR_SPACE_AFTER_VALUE,
        HDR_IGNORE_LINE,
        HDR_ALMOST_DONE,
        HDR_HEADER_ALMOST_DONE
    };

    bool NoMoreHeader = false;

    BoundCursor BC;
    HTTPHeaderParseState State = HDR_START;

    B >> BC;

    for (u_char C, C1; C = *BC, !!BC; BC++) {

        switch (State) {
            case HDR_START:

                Header.Value < BC, Header.Name < BC;
                Header.Valid = true;

                switch (C) {
                    case CR:
                        Header.Value > BC;
                        State = HDR_HEADER_ALMOST_DONE;
                        break;
                    case LF:
                        Header.Value > BC;
                        NoMoreHeader = true;
                        goto done;
                        break;
                    default:
                        State = HDR_NAME;
                        C1 = LowerCase[C];

                        if (C1) {
                            break;
                        }

                        if (C == '_') {
                            if (AllowUnderScore) {
                                break;
                            } else {
                                Header.Valid = false;
                            }
                            break;
                        }

                        if (C == '\0') {
                            return {EINVAL, InvalidHeaderErrorString};
                        }

                        Header.Valid = false;
                        break;
                }

                break;

            case HDR_NAME:

                C1 = LowerCase[C];

                if (C1) {
                    break;
                }

                if (C == '_') {

                    if (AllowUnderScore) {
                        break;
                    } else {
                        Header.Valid = false;

                    }
                    break;

                }

                if (C == ':') {

                    Header.Name > BC;
                    State = HDR_SPACE_BEFORE_VALUE;

                    break;
                }

                if (C == CR) {

                    Header.Name > BC;
                    Header.Value < BC;
                    Header.Value > BC;
                    State = HDR_ALMOST_DONE;

                    break;
                }

                if (C == LF) {

                    Header.Name > BC;
                    Header.Value < BC;
                    Header.Value > BC;

                    goto done;
                }

                if (C == '/') {

                    if (Header.Name.CmpBytes((u_char *)"HTTP", 4)) {
                        State = HDR_IGNORE_LINE;
                        break;
                    }

                }

                if (C == '\0') {
                    return {EINVAL, InvalidHeaderErrorString};
                }

                Header.Valid = false;
                break;

            case HDR_SPACE_BEFORE_VALUE:

                switch (C) {
                    case ' ':
                        break;
                    case CR:
                        Header.Value < BC;
                        Header.Value > BC;
                        State = HDR_ALMOST_DONE;

                        break;
                    case LF:
                        Header.Value < BC;
                        Header.Value > BC;

                        goto done;
                    case '\0':
                        return {EINVAL, InvalidHeaderErrorString};
                    default:
                        Header.Value < BC;
                        State = HDR_VALUE;

                        break;
                }

                break;
            case HDR_VALUE:

                switch (C) {
                    case ' ':
                        Header.Value > BC;
                        State = HDR_SPACE_AFTER_VALUE;

                        break;
                    case CR:
                        Header.Value > BC;
                        State = HDR_ALMOST_DONE;

                        break;
                    case LF:
                        Header.Value > BC;

                        goto done;
                    case '\0':
                        return {EINVAL, InvalidHeaderErrorString};
                }

                break;

            case HDR_SPACE_AFTER_VALUE:

                switch (C) {
                    case ' ':
                        break;
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

                switch (C) {
                    case LF:
                        State = HDR_START;
                        break;
                    default:
                        break;
                }

                break;
            case HDR_ALMOST_DONE:

                switch (C) {
                    case LF:
                        goto done;
                    case CR:
                        break;
                    default:
                        return {EINVAL, InvalidHeaderErrorString};
                }

                break;
            case HDR_HEADER_ALMOST_DONE:

                switch (C) {
                    case LF:
                        NoMoreHeader = true;
                        goto done;
                    default:
                        return {EINVAL, InvalidHeaderErrorString};
                }
        }
    }

    return {EAGAIN, BrokenHeaderErrorString};

    done:

    if (!(BC+1)) {
        return {EAGAIN, BrokenHeaderErrorString};
    }

    BC += 1;
    B << BC;

    return {0, NoMoreHeader? NoMoreHeaderErrorString: nullptr};
}

HTTPError HTTPRequest::ParseRequestHeaders(Buffer &B, HTTPRequest &R, bool AllowUnderScore) {

    HTTPError Error{0};
    HTTPHeader Header;

    // init header process if not
    if (HeaderInDictionary.Begin() == nullptr) {

        for (uint32_t i = 0; i < 31; i++) {
            HeaderInDictionary.Insert((HTTPCoreHeader &)HeaderInProcesses[i]);
        }

    }

    uint32_t Hash;

    do {

        Error = ParseHeader(B, Header, AllowUnderScore);

        if (Error.GetCode() != 0) {
            return Error;
        }

        if (Error.CodeMessage() == NoMoreHeaderErrorString) {
            break;
        }

        Hash = 0 ^ Header.Name.Size();

        for(BoundCursor BC = Header.Name; *BC != '\0'; BC ++) {
            SimpleHash(Hash, LowerCase[*BC]);
        }

        DictionaryItem *TempPrev, *TempNext, *DI = HeaderInDictionary.Find(Hash);

        if (DI != nullptr) {

            TempPrev = (DictionaryItem *)HeaderInDictionary.Prev(DI);
            TempNext = (DictionaryItem *)HeaderInDictionary.Next(DI);

            if ((TempPrev != nullptr && TempPrev->GetHash() == DI->GetHash()) ||
                (TempNext != nullptr && TempNext->GetHash() == DI->GetHash())) {
                // [TODO] Collision found, should add warning here
            }

            Error = ((HTTPCoreHeader *)DI)->Process(R, Header);

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

HTTPError HTTPRequest::ValidateRequestURI(Buffer *B, HTTPRequest &R) {

    enum URI_PARSE_STATE {
        URI_START = 0,
        URI_AFTER_SLASH_IN_URI,
        URI_CHECK_URI,
        URI_URI
    };

    BoundCursor BC = R.URI;
    URI_PARSE_STATE State = URI_START;

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

HTTPError HTTPRequest::ReadRequest(Buffer &B) {

    HTTPError Error(0);

    switch (State) {

        case HTTP_INIT_STATE:
            State = HTTP_PAESE_METHOD;
        case HTTP_PAESE_METHOD:

            Error = ParseMethod(B, *this);

            if (Error.GetCode() == 0) {
                State = HTTP_PARSE_REQUEST_LINE;
            } else {
                State = HTTP_BAD_REQUEST_STATE;
                break;
            }

        case HTTP_PARSE_REQUEST_LINE:

            Error = ParseRequestLine(B, *this);

            if (Error.GetCode() == 0) {
                State = HTTP_PARSE_HEADER;
            } else {
                State = HTTP_BAD_REQUEST_STATE;

                break;
            }

        case HTTP_PARSE_HEADER:

            do {
                Error = ParseRequestHeaders(B, *this);
            } while (Error.GetCode() == 0 && Error.CodeMessage() == nullptr);

            if (Error.GetCode() == 0) {

                State = HTTP_HEADER_DONE;
            } else {
                State = HTTP_BAD_REQUEST_STATE;

                break;
            }

        case HTTP_HEADER_DONE:
            // to parse done or read chunk
            break;
        case HTTP_BAD_REQUEST_STATE:
        default:
            Error = {EINVAL, BadRequestErrorString};
            break;
    }

    return Error;

}

void HTTPRequest::Reset() {

}
