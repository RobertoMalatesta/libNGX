#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

const char BrokenHeaderErrorString[] = "Broken Header in buffer";
const char NoMoreHeaderErrorString[] = "No more header";
const char InvalidHeaderErrorString[] = "Invalid Header";

HTTPError HTTPHeader::Read(Buffer &B, bool AllowUnderscore) {

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

                Value < BC, Name < BC;
                Valid = true;

                switch (C) {
                    case CR:
                        Value > BC;
                        State = HDR_HEADER_ALMOST_DONE;
                        break;
                    case LF:
                        Value > BC;
                        NoMoreHeader = true;
                        goto done;
                        break;
                    default:
                        State = HDR_NAME;
                        C1 = lower_case[C];

                        if (C1) {
                            break;
                        }

                        if (C == '_') {
                            if (AllowUnderscore) {
                                break;
                            } else {
                                Valid = false;
                            }
                            break;
                        }

                        if (C == '\0') {
                            return {EINVAL, InvalidHeaderErrorString};
                        }

                        Valid = false;
                        break;
                }

                break;

            case HDR_NAME:

                C1 = lower_case[C];

                if (C1) {
                    break;
                }

                if (C == '_') {

                    if (AllowUnderscore) {
                        break;
                    } else {
                        Valid = false;

                    }
                    break;

                }

                if (C == ':') {

                    Name > BC;
                    State = HDR_SPACE_BEFORE_VALUE;

                    break;
                }

                if (C == CR) {

                    Name > BC;
                    Value < BC;
                    Value > BC;
                    State = HDR_ALMOST_DONE;

                    break;
                }

                if (C == LF) {

                    Name > BC;
                    Value < BC;
                    Value > BC;

                    goto done;
                }

                if (C == '/') {

                    if (Name.CmpBytes((u_char *) "HTTP", 4)) {
                        State = HDR_IGNORE_LINE;
                        break;
                    }

                }

                if (C == '\0') {
                    return {EINVAL, InvalidHeaderErrorString};
                }

                Valid = false;
                break;

            case HDR_SPACE_BEFORE_VALUE:

                switch (C) {
                    case ' ':
                        break;
                    case CR:
                        Value < BC;
                        Value > BC;
                        State = HDR_ALMOST_DONE;

                        break;
                    case LF:
                        Value < BC;
                        Value > BC;

                        goto done;
                    case '\0':
                        return {EINVAL, InvalidHeaderErrorString};
                    default:
                        Value < BC;
                        State = HDR_VALUE;

                        break;
                }

                break;
            case HDR_VALUE:

                switch (C) {
                    case ' ':
                        Value > BC;
                        State = HDR_SPACE_AFTER_VALUE;

                        break;
                    case CR:
                        Value > BC;
                        State = HDR_ALMOST_DONE;

                        break;
                    case LF:
                        Value > BC;

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

    if (!(BC + 1)) {
        return {EAGAIN, BrokenHeaderErrorString};
    }

    BC += 1;
    B << BC;

    return {NoMoreHeader ? ENOENT : 0, nullptr};
}
