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
            char TimeBuf[HttpTimeSize];
            WriteHttpTime(TimeBuf, HttpTimeSize);
            printf("%s\n", TimeBuf);
        }

        {
            char TimeBuf[HttpLogTimeSize];
            WriteHttpLogTime(TimeBuf, HttpLogTimeSize);
            printf("%s\n", TimeBuf);
        }

        {
            char TimeBuf[HttpLogTimeISO8601Size];
            WriteHttpLogTimeISO8601(TimeBuf, HttpLogTimeISO8601Size);
            printf("%s\n", TimeBuf);
        }

        {
            char TimeBuf[SysLogTimeSize];
            WriteSysLogTime(TimeBuf, SysLogTimeSize);
            printf("%s\n", TimeBuf);
        }

        ForceUSleep(1000 * 1000);
    }

    return 0;
}
