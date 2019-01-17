#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

const char BadResponseLineErrorString[] = "bad response line error";
const char BrokenResponseLineErrorString[] = "broken response line error";

HTTPCoreHeader HTTPResponse::HeaderOutProcesses[] = {
        {nullptr, HI_COMMON, nullptr},
};

HTTPError HTTPResponse::ParseResponseLine(Buffer &B, HTTPResponse &R) {

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

    u_char C, C1;
    u_short HTTPMajor = 0, HTTPMinor = 0;
    BoundCursor BC, LastBC;

    for (B >> BC, LastBC = BC; C = *BC, !!BC; LastBC = BC++) {

        switch (ResponseLineState) {

            case RL_START:

                switch (C) {
                    case 'H':
                        ResponseLineState = RL_H;
                        break;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }

                break;
            case RL_H:

                switch (C) {
                    case 'T':
                        ResponseLineState = RL_HT;
                        break;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }

                break;
            case RL_HT:

                switch (C) {
                    case 'T':
                        ResponseLineState = RL_HTT;
                        break;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }

                break;
            case RL_HTT:

                switch (C) {
                    case 'P':
                        ResponseLineState = RL_HTTP;
                        break;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }

                break;
            case RL_HTTP:

                switch (C) {
                    case '/':
                        ResponseLineState = RL_FIRST_MAJOR_DIGIT;
                        break;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }

                break;
            case RL_FIRST_MAJOR_DIGIT:

                if (C < '1' || C > '9') {
                    return {EINVAL, BadResponseLineErrorString};
                }

                HTTPMajor = C - '0', ResponseLineState = RL_MAJOR_DIGIT;
                break;
            case RL_MAJOR_DIGIT:

                if (C == '.') {
                    ResponseLineState = RL_FIRST_MINOR_DIGIT;
                    break;
                }

                if (C < '1' || C > '9') {
                    return {EINVAL, BadResponseLineErrorString};
                }

                if (HTTPMajor > 99) {
                    return {EINVAL, BadResponseLineErrorString};
                }

                HTTPMajor = 10 * HTTPMajor + (C - '0');
                break;
            case RL_FIRST_MINOR_DIGIT:

                if (C < '1' || C > '9') {
                    return {EINVAL, BadResponseLineErrorString};
                }

                HTTPMinor = C - '0', ResponseLineState = RL_MINOR_DIGIT;
                break;
            case RL_MINOR_DIGIT:

                if (C == ' ') {

                    R.StatusCode < (BC + 1);
                    ResponseLineState = RL_STATUS;
                    break;
                }

                if (C < '1' || C > '9') {
                    return {EINVAL, BadResponseLineErrorString};
                }

                if (HTTPMinor > 99) {
                    return {EINVAL, BadResponseLineErrorString};
                }

                HTTPMinor = 10 * HTTPMinor + (C - '0');
                break;
            case RL_STATUS:

                if (C == ' ') {
                    ResponseLineState = RL_STATUS;
                    break;
                }

                if (C < '1' || C > '9') {
                    return {EINVAL, BadResponseLineErrorString};
                }

                R.Status = R.Status * 10 + (C - '0');
                R.StatusCode > BC;

                if (R.StatusCode.Size() == 3 || R.Status > 999) {
                    ResponseLineState = RL_SPACE_AFTER_STATUS;
                    break;
                }
                break;
            case RL_SPACE_AFTER_STATUS:

                switch (C) {
                    case ' ':
                    case '.':
                        ResponseLineState = RL_STATUS_TEXT;
                        R.StatusText < (BC) + 1;
                        break;
                    case CR:
                        ResponseLineState = RL_ALMOST_DONE;
                        break;
                    case LF:
                        goto done;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }
                break;
            case RL_STATUS_TEXT:
                switch (C) {
                    case CR:
                        ResponseLineState = RL_ALMOST_DONE;
                        break;
                    case LF:
                        goto done;
                }
                R.StatusText > BC;
                break;
            case RL_ALMOST_DONE:
                switch (C) {
                    case LF:
                        goto done;
                    default:
                        return {EINVAL, BadResponseLineErrorString};
                }
        }
    }

    return {EINVAL, BrokenResponseLineErrorString};

    done:

    if (!(BC + 1)) {
        return {EAGAIN, BrokenResponseLineErrorString};
    }

    BC += 1, B << BC;
    R.Version = HTTPMajor * 1000 + HTTPMinor;

    return {0};
}

HTTPError HTTPResponse::Read(Buffer &B) {
    return {0};
}

HTTPError HTTPResponse::Write(Buffer &B) {
    return {0};
}

void HTTPResponse::Reset() {

}