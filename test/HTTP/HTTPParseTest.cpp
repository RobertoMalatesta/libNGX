#include "Core/Support/Support.h"
#include "HTTP/HTTPError.h"
#include "HTTP/HTTPRequest.h"
#include <gtest/gtest.h>
#include <iostream>
using namespace ngx::Core::Support;
using namespace ngx::HTTP;
using namespace std;

TEST(HTTP, parseRequest) {
    WritableMemoryBuffer buf;
    BufferWriter writer(buf);
    HTTPRequest request;

    char Text[] = "GET /?argument=1 HTTP/1.1\r\n ";
    writer.fromString(Text, sizeof(Text));
    HTTPRequest::ParseMethod(buf, request);
    HTTPRequest::ParseRequestLine(buf, request);

    std::cout<< request.HTTPProtocol.size() << std::endl;

}
