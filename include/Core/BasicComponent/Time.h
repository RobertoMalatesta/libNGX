const uint32_t U_SECOND_SIZE = 1000000;
const uint32_t NANO_SECOND_SIZE = 1000000000;
const size_t ERROR_LOG_TIME_SIZE = sizeof("1970/09/28 12:00:00");
const size_t HTTP_TIME_SIZE = sizeof("Mon, 28 Sep 1970 06:00:00 GMT");
const size_t HTTP_LOG_TIME_SIZE = sizeof("28/Sep/1970:12:00:00 +0600");
const size_t HTTP_LOG_TIME_ISO8601_SIZE = sizeof("28/Sep/1970:12:00:00 +0600");
const size_t SYS_LOG_TIME_SIZE = sizeof("Sep 28 12:00:00");

bool IsInterrupted();

void ClearInterrupted();

int TimeModuleInit();

int ForceSleep(uint64_t NanoSeconds);

int StartTimer();

uint64_t GetHighResolutionTimestamp();

uint64_t GetTimeStamp();

int WriteErrorLogTime(char *Buf, size_t Size);

int WriteHTTPTime(char *Buf, size_t Size);

int WriteHTTPLogTime(char *Buf, size_t Size);

int WriteHTTPLogTimeISO8601(char *Buf, size_t Size);

int WriteSysLogTime(char *Buf, size_t Size);
