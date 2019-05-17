namespace ngx {
    namespace HTTP {
        using namespace ngx::Core::Support;
        typedef HTTPError (HTTPHeaderProcess)(HTTPCoreHeader &, HTTPRequest &, HTTPHeader &H);

        struct HTTPHeader {
            BoundCursor Name;
            BoundCursor Value;
            bool Valid;

            HTTPHeader() : Name(), Value(), Valid(true) {}

            HTTPError Read(Buffer &B, bool AllowUnderscore);

            HTTPError Write(Buffer &B);
        };

        enum HTTPCoreHeaderIn {
            HI_COMMON = -1,
            HI_HOST = 0,
            HI_CONNECTION,
            HI_IF_MODIFY_SINCE,
            HI_IF_UNMODIFY_SINCE,
            HI_IF_MATCH,
            HI_IF_NON_MATCH,
            HI_USERAGENT,
            HI_REFERENCE,
            HI_CONTENT_LENGTH,
            HI_CONTENT_RANGE,
            HI_CONTENT_TYPE,
            HI_RANGE,
            HI_IF_RANGE,
            HI_TRANSFER_ENCODING,
            HI_TE,
            HI_EXPECT,
            HI_UPGRADE,
            HI_ACCEPT_ENCODING,
            HI_VIA,
            HI_AUTHORIZATION,
            HI_KEEPALIVE,
            HI_X_FORWARD_FOR,
            HI_X_REAL_IP,
            HI_ACCEPT,
            HI_ACCEPT_LANGUAGE,
            HI_DEPTH,
            HI_DESTINATION,
            HI_OVERWRITE,
            HI_DATE,
            HI_COOKIE,
        };

        enum HTTPCoreHeaderOut {
            HO_COMMON = -1,
            HO_HOST = 0,
        };
    } //    HTTP
}   // ngx

