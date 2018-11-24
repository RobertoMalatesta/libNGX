#include "Core/Core.h"

#include <unistd.h>

using namespace std;
using namespace ngx::Core;

int TimeTest() {

    TimeModuleInit();
    GetTimeStamp();

    printf("Testing Time module...\n");

    for (int i = 0; i < 10; i++) {
        printf("-----------------------------------------\n");

        {
            char TimeBuf[ERROR_LOG_TIME_SIZE];
            memset(TimeBuf, 0, ERROR_LOG_TIME_SIZE);
            WriteErrorLogTime(TimeBuf, ERROR_LOG_TIME_SIZE);
            printf("%s\n", TimeBuf);
        }

        {
            char TimeBuf[HTTP_TIME_SIZE];
            WriteHTTPTime(TimeBuf, HTTP_TIME_SIZE);
            printf("%s\n", TimeBuf);
        }

        {
            char TimeBuf[HTTP_LOG_TIME_SIZE];
            WriteHTTPLogTime(TimeBuf, HTTP_LOG_TIME_SIZE);
            printf("%s\n", TimeBuf);
        }

        {
            char TimeBuf[HTTP_LOG_TIME_ISO8601_SIZE];
            WriteHTTPLogTimeISO8601(TimeBuf, HTTP_LOG_TIME_ISO8601_SIZE);
            printf("%s\n", TimeBuf);
        }

        {
            char TimeBuf[SYS_LOG_TIME_SIZE];
            WriteSysLogTime(TimeBuf, SYS_LOG_TIME_SIZE);
            printf("%s\n", TimeBuf);
        }

        ForceSleep(NANO_SECOND_SIZE);
    }

    return 0;
}
