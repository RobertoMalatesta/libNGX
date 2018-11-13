const size_t ErrorLogTimeSize = sizeof("1970/09/28 12:00:00");
const size_t HTTPTimeSize = sizeof("Mon, 28 Sep 1970 06:00:00 GMT");
const size_t HTTPLogTimeSize = sizeof("28/Sep/1970:12:00:00 +0600");
const size_t HTTPLogTimeISO8601Size = sizeof("28/Sep/1970:12:00:00 +0600");
const size_t SysLogTimeSize = sizeof("Sep 28 12:00:00");

int TimeModuleInit();

int EnableTimer();

int DisableTimer();

uint64_t GetTimeStamp();

int WriteErrorLogTime(char *Buf, size_t Size);

int WriteHTTPTime(char *Buf, size_t Size);

int WriteHTTPLogTime(char *Buf, size_t Size);

int WriteHTTPLogTimeISO8601(char *Buf, size_t Size);

int WriteSysLogTime(char *Buf, size_t Size);
