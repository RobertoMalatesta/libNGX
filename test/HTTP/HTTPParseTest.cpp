#include "Core/Core.h"
#include "HTTP/HTTP.h"
#include "HTTP/HTTPError.h"
#include "HTTP/HTTPRequest.h"
using namespace ngx::Core::Support;
using namespace ngx::HTTP;

int HTTPParseTest() {

    Pool MemoryAllocator;
    Buffer buffer;
    BufferBuilder Builder(BUFFER_MEMORY_BLOCK_SIZE, 1000);
    HTTPRequest Request;
    BoundCursor BC;

    u_char Text[] = "GET / HTTP/1.1\r\n ";

    Builder.BuildBuffer(buffer);
    buffer.ReadBytes(Text, sizeof(Text) - 1);

    buffer >> BC;

    HTTPRequest R;

    R.Read(buffer);

//    HTTPHeader H;
//    H.Name = BC;
//
//    HTTPParser::ParseRequestHeaders(buffer, R);

//    HTTPError Error = HTTPParser::ParseHTTPRequest(buffer, Request);
//
//    if (Error.GetCode() != 0) {
//        printf("parse failed, error: %s\n", Error.CodeMessage());
//    }

    return 0;
}
