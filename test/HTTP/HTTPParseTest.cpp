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
    char Text[] = "GET http://baidu.com:8080/request_url/some_argument%20?argument=1 HTTP/1.1\r\n ";
    writer.fromString(Text, sizeof(Text));
    request.parseMethod(buf).PrintError();
    request.parseRequestLine(buf).PrintError();

    for(auto &i : request.MethodStr) {
        cout<<i;
    }
    cout<<endl;

    for(auto &i : request.URI) {
        cout<<i;
    }
    cout<<endl;

    for(auto &i : request.Protocol) {
        cout<<i;
    }
    cout<<endl;

}
