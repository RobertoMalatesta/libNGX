#ifndef NGX_HTTP_RESPONSE
#define NGX_HTTP_RESPONSE

#include "Core/Core.h"

namespace ngx {
    namespace HTTP {

        using namespace ngx::Core::Support;

        struct HTTPHeaderOut {
            // HTTP Core Headers
            // HTTP Custom Headers
            Array <HTTPHeader> Headers;
            Array <HTTPHeader> Trailers;

            HTTPHeaderOut(Allocator *BackendAllocator) : Headers(BackendAllocator), Trailers(BackendAllocator) {};
        };

        class HTTPResponse {
        protected:

            enum HTTPResponseState {
                HTTP_BAD_RESPONSE = -1,
                HTTP_INIT,
                HTTP_PARSE_RESPONSE_LINE,
                HTTP_PARSE_HEADERS,
                HTTP_HEADER_DONE,
            };

            // HTTPResponse Parse State
            HTTPResponseState State = HTTP_INIT;

            int Status;
            int Version;
            BoundCursor StatusCode;
            BoundCursor StatusText;
            HTTPHeaderOut HeaderOut;

            static HTTPCoreHeader HeaderOutProcesses[];

            /** Parse HTTP Response Line from Buffer to HTTPResponse */
            static HTTPError ParseResponseLine(Buffer &B, HTTPResponse &R);

        public:

            HTTPResponse(Allocator *Allocator) : HeaderOut(Allocator) {
                Reset();
            };

            /** Read a request from buffer */
            HTTPError Read(Buffer &B);

            /** Write a request to buffer */
            HTTPError Write(Buffer &B);

            /** Reset state and release memory */
            virtual void Reset();
        };
    }
};
#endif