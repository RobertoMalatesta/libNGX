#include "Http/Http.h"

using namespace ngx::Http;

void inline HttpParseRequestLine();

void inline HttpProcessRequestHeader();

void inline HttpValidateHost();

void inline HttpProcessRequestURI();

void inline ResetState();


HttpError HttpRequestContext::ProcessHttpRequest(Buffer &B) {

    SpinlockGuard LockGuard(&Lock);

    HttpError Error(0);

    switch (State) {
        case HTTP_INIT_STATE:
            RequestLineState = RL_Start;
            Error = ParseRequestLine(B);
            if (Error.GetErrorCode() != 0) {
                return Error;
            }
            break;
        default:
            State = HTTP_INIT_STATE;
            return HttpError(EINVAL);
    }

    return HttpError(0);
}

HttpError HttpRequestContext::ParseRequestLine(Buffer &B) {

    BufferCursor ReadCursor = B.GetReadCursor();
    u_char C, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10;

    for (BufferCursor BC = B.GetReadCursor(); (C = *BC) != '\0'; BC++) {
        switch (RequestLineState) {
            case RL_Start:
                if (C == CR || C == LF) {
                    break;
                }
                if ((C<'A' || C > 'Z') && C != '_' && C != '-') {
                    return HttpError(EINVAL, "Invalid method!");
                }
                RequestLineState = RL_Method;
            case RL_Method:
                C1 = C, C2 = *(BC+1), C3 = *(BC+2), C4 = *(BC+3);
                if (C4 == ' ') {
                    if (C1 == 'G' && C2 == 'E' && C3 == 'T') {
                        Method = GET;
                    } else if (C1 == 'P' && C2 == 'U' && C3 == 'T') {
                        Method = PUT;
                    } else {
                        return HttpError(EINVAL, "Invalid method!");
                    }
                    BC += 3;
                } else if ((C5 = *(BC+4)) == ' ') {
                    if (C2 == 'O') {
                        if (C1 == 'P' && C3 == 'S' && C4 == 'T') {
                            Method = POST;
                        } else if (C1 == 'C' && C3 == 'P' && C4 == 'Y') {
                            Method = COPY;
                        } else if (C1 == 'M' && C3 == 'V' && C4 == 'E') {
                            Method = MOVE;
                        } else if (C1 == 'L' && C3 == 'C' && C4 == 'K') {
                            Method = LOCK;
                        }
                    } else if (C1 == 'H' && C2 == 'E' && C3 == 'A' && C4 == 'D') {
                        Method = HEAD;
                    } else {
                        return HttpError(EINVAL, "Invalid method!");
                    }
                    BC += 4;
                } else if ((C6 = *(BC+5)) == ' ') {
                    if (C1 == 'M' && C2 == 'K' && C3 == 'C' && C4 == 'O' && C5 == 'L') {
                        Method = MKCOL;
                    } else if (C1 == 'P' && C2 == 'A' && C3 == 'T' && C4 == 'C' && C5 == 'H') {
                        Method = PATCH;
                    } else if (C1 == 'T' && C2 == 'R' && C3 == 'A' && C4 == 'C' && C5 == 'E') {
                        Method = TRACE;
                    } else {
                        return HttpError(EINVAL, "Invalid method!");
                    }
                    BC += 5;
                } else if ((C7 = *(BC+6)) == ' ') {
                    if (C1 == 'D' && C2 == 'E' && C3 == 'L' && C4 == 'E' && C5 == 'T' && C6 == 'E') {
                        Method = DELETE;
                    } else if (C1 == 'U' && C2 == 'N' && C3 == 'L' && C4 == 'O' && C5 == 'C' && C6 == 'K') {
                        Method = UNLOCK;
                    } else {
                        return HttpError(EINVAL, "Invalid method!");
                    }
                    BC += 6;
                }
                else if (C1 == 'O' && C2 == 'P' && C3 == 'T' && C4 == 'I' &&
                    C5 == 'O' && C6 == 'N' && C7 == 'S' && (C8=*(BC+7)) == ' ') {
                    Method = OPTIONS, BC += 7;
                }
                else if (C1 == 'P' && C2 == 'R' && C3 == 'O' && C4 == 'P' &&
                    C5 == 'F' && C6 == 'I' && C7 == 'N' && C8 == 'D' && (C9=*(BC+8)) == ' ') {
                    Method = PROPFIND, BC += 8;
                } else if (C1 == 'P' && C2 == 'R' && C3 == 'O' && C4 == 'P' &&
                    C5 == 'P' && C6 == 'A' && C7 == 'T' && C8 == 'C' && C9 == 'H' && (C10=*(BC+9)) == ' ') {
                    Method = PROPPATCH, BC += 9;
                } else {
                    return HttpError(EINVAL, "Invalid method!");
                }
                RequestLineState = RL_Space_Before_Uri;
                break;
            case RL_Space_Before_Uri:
                if (C == '/') {
                    URI.LeftBound = BC;
                    RequestLineState = RL_AfterSlashInUri;
                    break;
                }

                C1 = (u_char)(C | 0x20);

                if (C1 >= 'a' && C1 <= 'z') {
                    Schema.LeftBound = BC;
                    RequestLineState = RL_Schema;
                    break;
                }

                switch (C) {
                    case ' ':
                        break;
                    default:
                        return HttpError(EFAULT, "Read failed!");
                }
                break;
            case RL_Schema:

                C1 = (u_char)(C | 0x20);

                if (C1 >= 'a' && C1 <= 'z') {
                    break;
                }

                if ((C >= '0' && C <= '9') || C == '+' || C == '-' || C == '.') {
                    break;
                }

                switch (C) {
                    case ':':
                        Schema.RightBound = BC;
                        RequestLineState = RL_SchemaSlash;
                    default:
                        return HttpError(EFAULT, "Read failed!");
                }
                break;
            case RL_SchemaSlash:
                switch (C) {
                    case '/':
                        RequestLineState = RL_SchemaSlashSlash;
                        break;
                    default:
                        return HttpError(EFAULT, "Read failed!");
                }
                break;
            case RL_SchemaSlashSlash:
                switch (C) {
                    case '/':
                        RequestLineState = RL_HostStart;
                        break;
                    default:
                        return HttpError(EFAULT, "Read failed!");
                }
                break;
            case RL_HostStart:
                Host.LeftBound = BC;
                if (C == '[') {
                    RequestLineState = RL_HostIpLiterial;
                    break;
                }
                RequestLineState = RL_Host;
            case RL_Host:
                C1 = (u_char)(C | 0x20);
                if (C1 >= 'a' && C1 <= 'z') {
                    break;
                }
                if ((C1 >= '0' && C1 <= '0') || C1 == '.' || C1 == '-') {
                    break;
                }
            case RL_HostEnd:
                Host.RightBound = BC;
                switch (C) {
                    case ':':
                        RequestLineState = RL_Port;
                        break;
                    case '/':
                        URI.LeftBound = BC;
                        RequestLineState = RL_AfterSlashInUri;
                        break;
                    case ' ':
                        URI.LeftBound = BC+1, URI.RightBound = BC+2;
                        RequestLineState = RL_CheckUriHttp09;
                        break;
                    default:
                        return HttpError(EFAULT, "Read failed!");
                }
                break;
            case RL_HostIpLiterial:
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
                        RequestLineState = RL_HostEnd;
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
                        return HttpError(EFAULT, "Read failed!");
                }
                break;
            case RL_Port:
                if (C >= '0' && C <= '9') {
                    break;
                }
                switch (C) {
                    case '/':
                        Port.RightBound = URI.LeftBound = BC;
                        RequestLineState = RL_AfterSlashInUri;
                        break;
                    case ' ':
                        Port.RightBound = BC, URI.LeftBound = BC+1, URI.RightBound = BC+2;
                        RequestLineState = RL_HostHttp09;
                        break;
                    default:
                        return HttpError(EFAULT, "Read failed!");
                }
                break;
            case RL_HostHttp09:
                switch (C) {
                    case ' ':
                        break;
                    case CR:
                        Http_minor = 9;
                        RequestLineState = RL_AlmostDone;
                        break;
                    case LF:
                        Http_minor = 9;
//                        goto done;
                    case 'H':
//                        r->http_protocol.data = p;
//                        state = sw_http_H;i
                        RequestLineState = RL_Http_H;
                        break;
                    default:
                        return HttpError(EFAULT, "Read failed!");
                }
                break;
            case RL_AfterSlashInUri:
                printf("stuck here!\n");
                break;
            default:
                printf("not handled!");
        }
    }
    return HttpError(0);
}

void HttpRequestContext::Reset() {
    State = HTTP_INIT_STATE;
}

