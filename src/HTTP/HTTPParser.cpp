#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

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

HTTPError HTTPParser::ParseHTTPRequest(Buffer &B, HTTPRequest &R) {

    SpinlockGuard LockGuard(&R.Lock);

    HTTPError Error(0);

    switch (R.State) {
        case HTTP_INIT_STATE:
            R.State = HTTP_PAESE_METHOD;
        case HTTP_PAESE_METHOD:
            Error = ParseMethod(B, R);

            if (Error.GetErrorCode() != 0) {
                return Error;
            }

            R.State = HTTP_PARSE_REQUEST_LINE;
        case HTTP_PARSE_REQUEST_LINE:
            Error = ParseRequestLine(B, R);

            if (Error.GetErrorCode() != 0) {
                return Error;
            }

            R.State = HTTP_PARSE_REQUEST_LINE;
            break;
        default:
            R.State = HTTP_INIT_STATE;
            return {EINVAL};
    }

    return {0};
}

HTTPError HTTPParser::ParseMethod(Buffer &B, HTTPRequest &R) {

    u_char C, C1, C2, C3, C4, C5, C6, C7, C8, C9;
    BoundCursor BC;

    for (B >> BC; (C = *BC) != '\0'; BC++) {
        if (C == CR || C == LF) {
            continue;
        }
        if ((C < 'A' || C > 'Z') && C != '_' && C != '-') {
            return {EFAULT, "Invalid method!"};
        }
        break;
    }

    C1 = C, C2 = *(BC + 1), C3 = *(BC + 2), C4 = *(BC + 3);

    if (C4 == ' ') {
        if (C1 == 'G' && C2 == 'E' && C3 == 'T') {
            R.Method = GET;
        } else if (C1 == 'P' && C2 == 'U' && C3 == 'T') {
            R.Method = PUT;
        } else {
            return {EFAULT, "Invalid method!"};
        }
        BC += 4;
    } else if ((C5 = *(BC + 4)) == ' ') {
        if (C2 == 'O') {
            if (C1 == 'P' && C3 == 'S' && C4 == 'T') {
                R.Method = POST;
            } else if (C1 == 'C' && C3 == 'P' && C4 == 'Y') {
                R.Method = COPY;
            } else if (C1 == 'M' && C3 == 'V' && C4 == 'E') {
                R.Method = MOVE;
            } else if (C1 == 'L' && C3 == 'C' && C4 == 'K') {
                R.Method = LOCK;
            }
        } else if (C1 == 'H' && C2 == 'E' && C3 == 'A' && C4 == 'D') {
            R.Method = HEAD;
        } else {
            return {EFAULT, "Invalid method!"};
        }
        BC += 5;
    } else if ((C6 = *(BC + 5)) == ' ') {
        if (C1 == 'M' && C2 == 'K' && C3 == 'C' && C4 == 'O' && C5 == 'L') {
            R.Method = MKCOL;
        } else if (C1 == 'P' && C2 == 'A' && C3 == 'T' && C4 == 'C' && C5 == 'H') {
            R.Method = PATCH;
        } else if (C1 == 'T' && C2 == 'R' && C3 == 'A' && C4 == 'C' && C5 == 'E') {
            R.Method = TRACE;
        } else {
            return {EFAULT, "Invalid method!"};
        }
        BC += 6;
    } else if ((C7 = *(BC + 6)) == ' ') {
        if (C1 == 'D' && C2 == 'E' && C3 == 'L' && C4 == 'E' && C5 == 'T' && C6 == 'E') {
            R.Method = DELETE;
        } else if (C1 == 'U' && C2 == 'N' && C3 == 'L' && C4 == 'O' && C5 == 'C' && C6 == 'K') {
            R.Method = UNLOCK;
        } else {
            return {EFAULT, "Invalid method!"};
        }
        BC += 7;
    } else if (C1 == 'O' && C2 == 'P' && C3 == 'T' && C4 == 'I' &&
               C5 == 'O' && C6 == 'N' && C7 == 'S' && (C8 = *(BC + 7)) == ' ') {
        R.Method = OPTIONS, BC += 8;
    } else if (C1 == 'P' && C2 == 'R' && C3 == 'O' && C4 == 'P' &&
               C5 == 'F' && C6 == 'I' && C7 == 'N' && C8 == 'D' && (C9 = *(BC + 8)) == ' ') {
        R.Method = PROPFIND, BC += 9;
    } else if (C1 == 'P' && C2 == 'R' && C3 == 'O' && C4 == 'P' &&
               C5 == 'P' && C6 == 'A' && C7 == 'T' && C8 == 'C' && C9 == 'H' && (*(BC + 9)) == ' ') {
        R.Method = PROPPATCH, BC += 10;
    } else {
        return {EFAULT, "Invalid method!"};
    }

    B << BC;

    return  {0};
}

HTTPError HTTPParser::ParseRequestLine(Buffer &B, HTTPRequest &R) {

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
    BoundCursor BC,LastBC;

    for (B >> BC, LastBC = BC; (C = *BC) != '\0'; LastBC = BC++) {
        switch (RequestLineState) {
            case RL_SPACEBEFOREURI:
                if (C == '/') {
                    R.URI.LeftBound = BC;
                    RequestLineState = RL_AFTERSLASHINURI;
                    break;
                }

                C1 = (u_char) (C | 0x20);

                if (C1 >= 'a' && C1 <= 'z') {
                    R.Schema.LeftBound = BC;
                    RequestLineState = RL_SCHEMA;
                    break;
                }

                switch (C) {
                    case ' ':
                        break;
                    default:
                        return {EFAULT, "Bad Request!"};
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
                        R.Schema.RightBound = BC;
                        RequestLineState = RL_SCHEMASLASH;
                        break;
                    default:
                        return {EFAULT, "Bad Request!"};
                }
                break;
            case RL_SCHEMASLASH:
                switch (C) {
                    case '/':
                        RequestLineState = RL_SCHEMASLASHSLASH;
                        break;
                    default:
                        return {EFAULT, "Bad Request!"};
                }
                break;
            case RL_SCHEMASLASHSLASH:
                switch (C) {
                    case '/':
                        RequestLineState = RL_HOSTSTART;
                        break;
                    default:
                        return {EFAULT, "Bad Request!"};
                }
                break;
            case RL_HOSTSTART:
                R.Host.LeftBound = BC;
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
                R.Host.RightBound = BC;
                switch (C) {
                    case ':':
                        RequestLineState = RL_PORT;
                        break;
                    case '/':
                        R.URI.LeftBound = BC;
                        RequestLineState = RL_AFTERSLASHINURI;
                        break;
                    case ' ':
                        R.URI.LeftBound = BC + 1, R.URI.RightBound = BC + 2;
                        RequestLineState = RL_CHECKURIHTTP09;
                        break;
                    default:
                        return {EFAULT, "Bad Request!"};
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
                        return {EFAULT, "Bad Request!"};
                }
                break;
            case RL_PORT:
                if (C >= '0' && C <= '9') {
                    break;
                }
                switch (C) {
                    case '/':
                        R.Port.RightBound = R.URI.LeftBound = BC;
                        RequestLineState = RL_AFTERSLASHINURI;
                        break;
                    case ' ':
                        R.Port.RightBound = BC, R.URI.LeftBound = BC + 1, R.URI.RightBound = BC + 2;
                        RequestLineState = RL_HOSTHTTP09;
                        break;
                    default:
                        return {EFAULT, "Bad Request!"};
                }
                break;
            case RL_HOSTHTTP09:
                switch (C) {
                    case ' ':
                        break;
                    case CR:
                        R.HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        R.HTTPMinor = 9;
                        goto done;
                    case 'H':
                        R.HTTPProtocol.LeftBound = BC;
                        RequestLineState = RL_HTTP_H;
                        break;
                    default:
                        return {EFAULT, "Bad Request!"};
                }
                break;
            case RL_AFTERSLASHINURI:

                if (usual[C >> 5] & (1U << (C & 0x1f))) {
                    RequestLineState = RL_CHECKURI;
                    break;
                }

                switch (C) {
                    case ' ':
                        R.URI.RightBound = BC;
                        RequestLineState = RL_CHECKURIHTTP09;
                        break;
                    case CR:
                        R.URI.RightBound = BC;
                        R.HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        R.URI.RightBound = BC;
                        R.HTTPMinor = 9;
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
                        R.Arguments.LeftBound = BC + 1;
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
                        return {EFAULT, "Bad Request!"};
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
                        R.URI.RightBound = BC;
                        RequestLineState = RL_CHECKURIHTTP09;
                        break;
                    case CR:
                        R.URI.RightBound = BC;
                        R.HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        R.URI.RightBound = BC;
                        R.HTTPMinor = 9;
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
                        R.Arguments.LeftBound = BC + 1;
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
                        return {EFAULT, "Bad Request!"};
                }
                break;

            case RL_CHECKURIHTTP09:
                switch (C) {
                    case ' ':
                        break;
                    case CR:
                       R.HTTPMinor = 9;
                       RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        R.HTTPMinor = 9;
                        goto done;
                    case 'H':
                        R.HTTPProtocol.LeftBound = BC;
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
                        R.URI.RightBound = BC;
                        RequestLineState = RL_HTTP09;
                        break;
                    case CR:
                        R.URI.RightBound = BC;
                        R.HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        R.URI.RightBound = BC;
                        R.HTTPMinor = 9;
                        goto done;
                    case '#':
                        R.ComplexURI = 1;
                        break;
                    case '\0':
                        return {EFAULT, "Bad Request!"};
                }
                break;

                /* space+ after URI */
            case RL_HTTP09:
                switch (C) {
                    case ' ':
                        break;
                    case CR:
                        R.HTTPMinor = 9;
                        RequestLineState = RL_ALMOSTDONE;
                        break;
                    case LF:
                        R.HTTPMinor = 9;
                        goto done;
                    case 'H':
                        R.HTTPProtocol.LeftBound = BC;
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
                        return {EFAULT, "Bad Request!"};
                }
                break;

            case RL_HTTP_HT:
                switch (C) {
                    case 'T':
                        RequestLineState = RL_HTTP_HTT;
                        break;
                    default:
                        return {EFAULT, "Bad Request!"};
                }
                break;

            case RL_HTTP_HTT:
                switch (C) {
                    case 'P':
                        RequestLineState = RL_HTTP_HTTP;
                        break;
                    default:
                        return {EFAULT, "Bad Request!"};
                }
                break;

            case RL_HTTP_HTTP:
                switch (C) {
                    case '/':
                        RequestLineState = RL_FIRSTMAJORDIGIT;
                        break;
                    default:
                        return {EFAULT, "Bad Request!"};
                }
                break;

            case RL_FIRSTMAJORDIGIT:
                if (C < '1' || C > '9') {
                    return {EFAULT, "Bad Request!"};
                }
                R.HTTPMajor = C - '0';

                if (R.HTTPMajor > 1) {
                    return {EFAULT, "Invalid Version!"};
                }
                RequestLineState = RL_MAJORDIGIT;
                break;

            case RL_MAJORDIGIT:
                if (C == '.') {
                    RequestLineState = RL_FIRSTMINORDIGIT;
                    break;
                }

                if (C < '0' || C > '9') {
                    return {EFAULT, "Bad Request!"};
                }

                R.HTTPMajor = R.HTTPMajor * 10 + (C - '0');

                if (R.HTTPMajor > 1) {
                    return {EFAULT, "Invalid Version!"};
                }
                break;

            case RL_FIRSTMINORDIGIT:
                if (C < '0' || C > '9') {
                    return {EFAULT, "Bad Request!"};
                }

                R.HTTPMinor = C - '0';
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
                    return {EFAULT, "Bad Request!"};
                }

                if (R.HTTPMinor > 99) {
                    return {EFAULT, "Bad Request!"};
                }

                R.HTTPMinor = R.HTTPMinor * 10 + (C - '0');
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
                        return {EFAULT, "Bad Request!"};
                }
                break;

            case RL_ALMOSTDONE:
                R.Request.RightBound = LastBC;
                switch (C) {
                    case LF:
                        goto done;
                    default:
                        return {EFAULT, "Bad Request!"};
                }
                break;
        }
    }

    return {EAGAIN, "Broken request in buffer!"};

    done:

    B << BC;
    R.Request.RightBound = BC;
    R.Version = R.HTTPMajor * 1000 + R.HTTPMinor;

    if (R.Version == 9 && R.Method != GET) {
        return {EFAULT, "Invalid 09 Method!"};
    }
    return {0};
}

HTTPError HTTPParser::ParseHeaders(Buffer &B, HTTPRequest &R) {

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

    return {0};
}

