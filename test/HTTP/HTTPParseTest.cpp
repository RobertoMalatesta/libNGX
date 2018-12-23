#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

int HTTPParseTest() {

    Pool MemoryAllocator;
    Buffer buffer;
    BufferBuilder Builder(BUFFER_MEMORY_BLOCK_SIZE, 1000);
    HTTPRequest Request(&MemoryAllocator);
    BoundCursor BC;

    u_char Text[] = "Accept-Encoding: application/json\n\r\n";

    Builder.BuildBuffer(buffer);
    buffer.ReadData(Text, sizeof(Text) - 1);

    buffer >> BC;

    HTTPRequest R(&MemoryAllocator);

    HTTPHeader H;
    H.Name = BC;

    HTTPParser::ParseRequestHeaders(buffer, R);

//    HTTPError Error = HTTPParser::ParseHTTPRequest(buffer, Request);
//
//    if (Error.GetCode() != 0) {
//        printf("parse failed, error: %s\n", Error.CodeMessage());
//    }

    return 0;
}
