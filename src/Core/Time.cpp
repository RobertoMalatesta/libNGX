
#include "Core/Core.h"
#include <sys/time.h>
#include <csignal>

using namespace std;

namespace ngx::Core {

    static atomic_flag TimestampLock = ATOMIC_FLAG_INIT;
    static timeval Timestamp;
    static int TimestampVersion;
    static bool UpdateTimestamp;

    static struct {
        uint64_t Timestamp;
        char ErrorLogTime [ErrorLogTimeSize];
        char HttpTime [HttpTimeSize];
        char HttpLogTime [HttpLogTimeSize];
        char HttpLogTimeISO8601 [HttpLogTimeISO8601Size];
        char SysLogTime[SysLogTimeSize];
    } TimeStringRingBuffer[NumTimeSlot];

    static const char  *WeekString[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    static const char  *MonthString[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    static void TimerHandle(int);
    static void UpdateTimeString();

    static inline int FetchTimeVersion();

    int TimeModuleInit() {

        struct sigaction  sa;
        struct itimerval  itv;

        UpdateTimeString();

        memset(&sa, 0, sizeof(struct sigaction));
        sa.sa_handler = TimerHandle;
        sigemptyset(&sa.sa_mask);

        if (sigaction(SIGALRM, &sa, nullptr) == -1) {
            return -1;
        }

        itv.it_interval.tv_sec = TimeResolution / 1000;
        itv.it_interval.tv_usec = (TimeResolution % 1000) * 1000;
        itv.it_value.tv_sec = TimeResolution / 1000;
        itv.it_value.tv_usec = (TimeResolution % 1000 ) * 1000;

        if (setitimer(ITIMER_REAL, &itv, nullptr) == -1) {
            return -1;
        }

        return 0;
    }

    uint64_t GetTimeStamp() {

        uint64_t Ret;

        int Version=FetchTimeVersion();

        Ret = TimeStringRingBuffer[Version].Timestamp;

        return Ret;
    }

    int WriteErrorLogTime(char *Buf, size_t Size) {

        int Version=FetchTimeVersion();


        if (Size < ErrorLogTimeSize) {
            return 0;
        }

        memcpy(Buf, TimeStringRingBuffer[Version].ErrorLogTime, ErrorLogTimeSize);

        return ErrorLogTimeSize-1;
    }

    int WriteHttpTime(char *Buf, size_t Size) {

        int Version=FetchTimeVersion();

        if (Size < HttpTimeSize) {
            return 0;
        }

        memcpy(Buf, TimeStringRingBuffer[Version].HttpTime, HttpTimeSize);

        return HttpTimeSize-1;
    }

    int WriteHttpLogTime(char *Buf, size_t Size) {

        int Version=FetchTimeVersion();

        if (Size < HttpLogTimeSize) {
            return 0;
        }

        memcpy(Buf, TimeStringRingBuffer[Version].HttpLogTime, HttpLogTimeSize);

        return HttpLogTimeSize-1;
    }

    int WriteHttpLogTimeISO8601(char *Buf, size_t Size) {

        int Version=FetchTimeVersion();

        if (Size < HttpLogTimeISO8601Size) {
            return 0;
        }

        memcpy(Buf, TimeStringRingBuffer[Version].HttpLogTimeISO8601, HttpLogTimeISO8601Size);

        return HttpLogTimeISO8601Size-1;
    }

    int WriteSysLogTime(char *Buf, size_t Size) {

        int Version=FetchTimeVersion();

        if (Size < SysLogTimeSize) {
            return 0;
        }

        memcpy(Buf, TimeStringRingBuffer[Version].SysLogTime, SysLogTimeSize);

        return SysLogTimeSize-1;
    }

    static int FetchTimeVersion() {

        int Version=0;
        while(TimestampLock.test_and_set()) {
            RelaxMachine();
        }

        if (UpdateTimestamp) {
            UpdateTimeString();
        }

        Version = TimestampVersion;

        TimestampLock.clear();

        return Version;
    }

    static void TimerHandle(int) {
        UpdateTimestamp=true;
    }

    static void UpdateTimeString() {

        uint64_t  Ts;
        TimestampVersion=(TimestampVersion + 1)%NumTimeSlot;
        gettimeofday(&Timestamp, nullptr);

        Ts = (uint64_t)(Timestamp.tv_sec);
        TimeStringRingBuffer[TimestampVersion].Timestamp = Ts;

        int Year, Month, MonthDay, Hour, Minute, Second, DayOfWeek, Days, Leap, DayOfYear;

        Days = (int)(Ts / 86400);
        Second = (int)(Ts % 86400);

        if (Days > 2932896) {
            Days = 2932896;
            Second = 86399;
        }

        DayOfWeek = (4+Days) % 7;
        Hour = Second / 3600;
        Second = Second % 3600;
        Minute = Second / 60;
        Second = Second % 60;

        Days = Days - (31 + 28) + 719527;
        Year = (Days + 2) * 400 / (365 * 400 + 100 - 4 +1);
        DayOfYear = Days - (365 * Year + Year/4 - Year/100 + Year/400);

        if (DayOfYear < 0) {
            Leap = (Year % 4 == 0) && (Year % 100 || (Year % 400 == 0));
            DayOfYear = 365 + Leap + DayOfYear;
            Year --;
        }
        Month = (DayOfYear + 31) * 10 / 306;

        MonthDay = DayOfYear - (367 * Month / 12 - 30) + 1;

        if (DayOfYear >= 306) {
            Year ++;
            Month -= 10;
        }
        else {
            Month += 2;
        }

        sprintf(TimeStringRingBuffer[TimestampVersion].ErrorLogTime,
                "%4d/%02d/%02d %02d:%02d:%02d", Year, Month, MonthDay, Hour, Minute, Second);
        sprintf(TimeStringRingBuffer[TimestampVersion].HttpTime,
                "%s, %02d %s %4d %02d:%02d:%02d GMT",
                WeekString[DayOfWeek], MonthDay, MonthString[Month-1], Year, Hour, Minute, Second);
        sprintf(TimeStringRingBuffer[TimestampVersion].HttpLogTime,
                "%02d/%s/%d:%02d:%02d:%02d %c%02i%02i",
                MonthDay, MonthString[Month-1], Year, Hour, Minute, Second, '+', 8, 0);
        sprintf(TimeStringRingBuffer[TimestampVersion].HttpLogTimeISO8601,
                "%4d-%02d-%02dT%02d:%02d:%02d%c%02i:%02i", Year, Month, MonthDay, Hour, Minute, Second, '+', 8, 0);
        sprintf(TimeStringRingBuffer[TimestampVersion].SysLogTime,
                "%s %2d %02d:%02d:%02d", MonthString[Month-1], MonthDay, Hour, Minute, Second);

        UpdateTimestamp = false;
    }
}
