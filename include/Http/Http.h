#include "Core/Core.h"

namespace ngx::Http {
    using namespace ngx::Core;
    class HttpServer;
    class HttpConnection;
    class HttpRequestBuffer;
    class HttpResponseBuffer;
    struct HttpEventArgument;
    struct HttpPerformaceUint;
}

#include "Http/HttpServer.h"
#include "Http/HttpConnection.h"
#include "Http/HttpConnectionRecycler.h"
#include "Http/HttpBufferMemoryBlockRecycler.h"
#include "Http/HttpRequestBuffer.h"
#include "Http/HttpResponseBuffer.h"
