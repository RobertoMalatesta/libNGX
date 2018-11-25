#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

const ConstStringHash CoreHeaderInNames[HI_SIZE] = {
        {"Host", 0},
        {"Connection", 0},
        {"If-Midified-Since", 0},
        {"If-Unmodified-Since", 0},
        {"If-Match", 0},
        {"If-None-Match", 0},
        {"UserAgent", 0},
        {"Referer", 0},
        {"Content-Length", 0},
        {"Content-Range", 0},
        {"Content-Type", 0},
        {"Range", 0},
        {"If-Range", 0},
        {"Transfer-Encoding", 0},
        {"TE", 0},
        {"Expect", 0},
        {"Accept-Encoding", 0},
        {"Via", 0},
        {"Authorization", 0},
        {"Keep-Alive", 0},
        {"X-Forward-For", 0},
        {"X-Real-IP", 0},
        {"Accept", 0},
        {"Accept-Language", 0},
        {"Depth", 0},
        {"Destination", 0},
        {"Overwrite", 0},
        {"Date", 0},
        {"Cookie", 0},
};

HTTPCoreHeaderIn MatchHTTPHeaderHash(BoundCursor HeaderName) {
   return HI_INVALID;
}
