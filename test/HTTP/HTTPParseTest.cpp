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
    HTTPHeader header;
    char Text[] = "GET http://baidu.com:8080/request_url/some_argument%20?argument=1 HTTP/1.1\r\n"
                  "hOst: xxxx\r\n"
                  "accept-encoding: xxxx\r\n"
                  "Accept-encoding: xxxx\r\n"
                  "Content-type: xxxx\r\n"
                  "Content-Range: xxxx\r\n"
                  "content-range: xxxx\r\n"
                  "\r\n";
    writer.fromString(Text, sizeof(Text)-1);
    request.parse(buf, writer.size(), buf, writer.size() );
    cout << request.HeaderMap.size() <<endl;
    for(auto &i : request.MethodStr) cout<<i;
    cout<<endl;
    for(auto &i : request.URI) cout<<i;
    cout<<endl;
    for(auto &i : request.Protocol) cout<<i;
    cout<<endl;
    for(auto &i : request.Argument) cout<<i;
    cout<<endl;
    for(auto &i : request.Host) cout<<i;
    cout<<endl;
    for(auto &i : request.Port) cout<<i;
    cout<<endl;
    cout<< sizeof(HTTPRequest) << endl;
}
