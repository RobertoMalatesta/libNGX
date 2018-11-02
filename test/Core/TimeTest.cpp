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
            char TimeBuf[ErrorLogTimeSize];
            memset(TimeBuf, 0, ErrorLogTimeSize);
            WriteErrorLogTime(TimeBuf, ErrorLogTimeSize);
            printf("%s\n", TimeBuf);
        }

        {
            char TimeBuf[HTTPTimeSize];
            WriteHTTPTime(TimeBuf, HTTPTimeSize);
            printf("%s\n", TimeBuf);
        }

        {
            char TimeBuf[HTTPLogTimeSize];
            WriteHTTPLogTime(TimeBuf, HTTPLogTimeSize);
            printf("%s\n", TimeBuf);
        }

        {
            char TimeBuf[HTTPLogTimeISO8601Size];
            WriteHTTPLogTimeISO8601(TimeBuf, HTTPLogTimeISO8601Size);
            printf("%s\n", TimeBuf);
        }

        {
            char TimeBuf[SysLogTimeSize];
            WriteSysLogTime(TimeBuf, SysLogTimeSize);
            printf("%s\n", TimeBuf);
        }

        usleep(1000 * 1000);
    }

    return 0;
}
