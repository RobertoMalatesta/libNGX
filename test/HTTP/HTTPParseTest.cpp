#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

int HTTPParseTest() {

    Pool MemoryAllocator;
    Buffer buffer;
    BufferBuilder Builder(BUFFER_MEMORY_BLOCK_SIZE, 1000);
    HTTPRequest Request(&MemoryAllocator);

    u_char Text[] = "GET / HTTP/1.1\r\nAcceptEncoding: application/json\n\r\n";

    Builder.BuildBuffer(buffer);
    buffer.WriteDataToBuffer(Text, sizeof(Text) - 1);

    HTTPError Error = HTTPParser::ParseHTTPRequest(buffer, Request);

    if (Error.GetCode() != 0) {
        printf("parse failed, error: %s\n", Error.CodeMessage());
    }

    return 0;
}
