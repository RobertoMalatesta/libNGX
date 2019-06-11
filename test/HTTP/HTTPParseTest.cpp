#include "Core/Support/Support.h"
#include "HTTP/HTTPError.h"
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPResponse.h"
#include <gtest/gtest.h>
#include <iostream>
using namespace ngx::Core::Support;
using namespace ngx::HTTP;
using namespace std;

TEST(HTTP, parseRequest) {
    unique_ptr<WritableMemoryBuffer> buf(WritableMemoryBuffer::NewBuffer(1024, true));
    BufferWriter writer(*buf);
    HTTPRequest request;
    HTTPHeader header;
    char Text[] = "GET https://www.baidu.com/home/msg/data/personalcontent?callback=jQuery11020627269302204776_1560264948633&num=8&_req_seqid=&sid=&_=1560264948634 HTTP/1.1\r\n"
                  "Accept-Encoding: gzip, deflate, br\r\n"
                  "Content-type: xxxx\r\n"
                  "Content-Range: xxxx\r\n"
                  "content-range: xxxx\r\n"
                  "\r\n";
    writer.fromString(Text, sizeof(Text)-1);
    request.parse(writer.getData(), {}).PrintError();
    cout << request.HeaderMap.size() <<endl;
    for(auto &i : request.MethodStr) cout<<i;
    cout<<endl;
    for(auto &i : request.URI) cout<<i;
    cout<<endl;
    for(auto &i : request.RequestLine) cout<<i;
    cout<<endl;
    for(auto &i : request.Protocol) cout<<i;
    cout<<endl;
    for(auto &i : request.Argument) cout<<i;
    cout<<endl;
    for(auto &i : request.Host) cout<<i;
    cout<<endl;
    for(auto &i : request.Port) cout<<i;
    cout<<endl;
    for(auto &h : request.HeaderMap) {
        for(auto &i : h.second.getHeader()) cout<<i;
        cout<<": ";
        for(auto &i : h.second.getValue()) cout<<i;
        cout<<endl;
    }
}

TEST(HTTP, parseResponse) {
    unique_ptr<WritableMemoryBuffer> buf(WritableMemoryBuffer::NewBuffer(1024, true));
    BufferWriter writer(*buf);
    HTTPResponse response;
    HTTPHeader header;
    char Text[] = "HTTP/1.1 200 OK\r\n"
                  "Content-Length: 2000\r\n"
                  "Content-Type: application/json\r\n\r\n";
    writer.fromString(Text, sizeof(Text)-1);
    response.parse(writer.getData(), {}).PrintError();
    cout << response.getHeaders().size() <<endl;
    for(auto &i : response.getStatusCodeString()) cout<<i;
    cout<<endl;
    for(auto &i : response.getStatusText()) cout<<i;
    cout<<endl;
    for(auto &i : response.getStatusLine()) cout<<i;
    cout<<endl;
}
