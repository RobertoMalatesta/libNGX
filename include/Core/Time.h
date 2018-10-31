namespace ngx {
    namespace Core {
        const size_t ErrorLogTimeSize = sizeof("1970/09/28 12:00:00");
        const size_t HttpTimeSize = sizeof("Mon, 28 Sep 1970 06:00:00 GMT");
        const size_t HttpLogTimeSize = sizeof("28/Sep/1970:12:00:00 +0600");
        const size_t HttpLogTimeISO8601Size = sizeof("28/Sep/1970:12:00:00 +0600");
        const size_t SysLogTimeSize = sizeof("Sep 28 12:00:00");

        int TimeModuleInit();
        int EnableTimer();
        int DisableTimer();
        void ForceUSleep(useconds_t USeconds);
        uint64_t GetTimeStamp();
        int WriteErrorLogTime(char *Buf, size_t Size);
        int WriteHttpTime(char *Buf, size_t Size);
        int WriteHttpLogTime(char *Buf, size_t Size);
        int WriteHttpLogTimeISO8601(char *Buf, size_t Size);
        int WriteSysLogTime(char *Buf, size_t Size);
    }
}