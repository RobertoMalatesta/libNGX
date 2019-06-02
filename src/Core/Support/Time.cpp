#include "Core/Support/Time.h"
#include <csignal>
#include <unistd.h>
#include <sys/time.h>
#include <cstring>

using namespace std;
using namespace ngx::Core;
using namespace ngx::Core::Config;
using namespace ngx::Core::Target::X86;
using namespace ngx::Core::Support;

static spin_lock Lock;
static timeval Timestamp;
static int TimestampVersion;
static bool UpdateTimestamp;
static bool Interrupted;

static uint64_t HighResolutionTimestamp;

static struct {
    uint64_t Timestamp;
    char ErrorLogTime[ERROR_LOG_TIME_SIZE];
    char HTTPTime[HTTP_TIME_SIZE];
    char HTTPLogTime[HTTP_LOG_TIME_SIZE];
    char HTTPLogTimeISO8601[HTTP_LOG_TIME_ISO8601_SIZE];
    char SysLogTime[SYS_LOG_TIME_SIZE];
} TimeStringRingBuffer[NUM_TIME_SLOT];

static const char *WeekString[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *MonthString[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static void TimerHandle(int);

static void InterruptHandle(int);

static void UpdateTimeString();

static inline int FetchTimeVersion(bool Force = false);

bool ngx::Core::Support::IsInterrupted() {
    return Interrupted;
}

void ngx::Core::Support::ClearInterrupted() {
    Interrupted = false;
}

int ngx::Core::Support::TimeModuleInit() {

    struct sigaction sa = {nullptr};

    static bool TimeModuleInited = false;

    if (!TimeModuleInited) {
        UpdateTimeString();

        memset(&sa, 0, sizeof(struct sigaction));
        sa.sa_handler = SIG_IGN;

        if (sigaction(SIGPIPE, &sa, nullptr) == -1) {
            return -1;
        }

        memset(&sa, 0, sizeof(struct sigaction));
        sa.sa_handler = TimerHandle;
        sigemptyset(&sa.sa_mask);

        if (sigaction(SIGALRM, &sa, nullptr) == -1) {
            return -1;
        }

        memset(&sa, 0, sizeof(struct sigaction));
        sa.sa_handler = InterruptHandle;
        sigemptyset(&sa.sa_mask);

        if (sigaction(SIGINT, &sa, nullptr) == -1) {
            return -1;
        }

        if (StartTimer() == -1) {
            return -1;
        }

        Interrupted = false;

        TimeModuleInited = true;
        FetchTimeVersion();
    }
    return 0;
}

int ngx::Core::Support::StartTimer() {

    struct itimerval itv = {0};

    itv.it_interval.tv_sec = TIME_RESOLUTION / U_SECOND_SIZE;
    itv.it_interval.tv_usec = (TIME_RESOLUTION % U_SECOND_SIZE);
    itv.it_value.tv_sec = TIME_RESOLUTION / U_SECOND_SIZE;
    itv.it_value.tv_usec = (TIME_RESOLUTION % U_SECOND_SIZE);

    if (setitimer(ITIMER_REAL, &itv, nullptr) == -1) {
        return -1;
    }
    return 0;
}

uint64_t ngx::Core::Support::GetHighResolutionTimestamp() {

    std::lock_guard<spin_lock> g(Lock);

    if (UpdateTimestamp) {
        UpdateTimeString();
    }

    return HighResolutionTimestamp;
}

uint64_t ngx::Core::Support::GetTimeStamp() {

    uint64_t Ret;

    int Version = FetchTimeVersion();

    Ret = TimeStringRingBuffer[Version].Timestamp;

    return Ret;
}

int ngx::Core::Support::WriteErrorLogTime(char *Buf, size_t Size) {

    int Version = FetchTimeVersion();


    if (Size < ERROR_LOG_TIME_SIZE) {
        return 0;
    }

    memcpy(Buf, TimeStringRingBuffer[Version].ErrorLogTime, ERROR_LOG_TIME_SIZE - 1);
    return ERROR_LOG_TIME_SIZE - 1;
}

int ngx::Core::Support::WriteHTTPTime(char *Buf, size_t Size) {

    int Version = FetchTimeVersion();

    if (Size < HTTP_TIME_SIZE) {
        return 0;
    }

    memcpy(Buf, TimeStringRingBuffer[Version].HTTPTime, HTTP_TIME_SIZE - 1);
    return HTTP_TIME_SIZE - 1;
}

int ngx::Core::Support::WriteHTTPLogTime(char *Buf, size_t Size) {

    int Version = FetchTimeVersion();

    if (Size < HTTP_LOG_TIME_SIZE) {
        return 0;
    }

    memcpy(Buf, TimeStringRingBuffer[Version].HTTPLogTime, HTTP_LOG_TIME_SIZE - 1);
    return HTTP_LOG_TIME_SIZE - 1;
}

int ngx::Core::Support::WriteHTTPLogTimeISO8601(char *Buf, size_t Size) {

    int Version = FetchTimeVersion();

    if (Size < HTTP_LOG_TIME_ISO8601_SIZE) {
        return 0;
    }

    memcpy(Buf, TimeStringRingBuffer[Version].HTTPLogTimeISO8601, HTTP_LOG_TIME_ISO8601_SIZE - 1);
    return HTTP_LOG_TIME_ISO8601_SIZE - 1;
}

int ngx::Core::Support::WriteSysLogTime(char *Buf, size_t Size) {

    int Version = FetchTimeVersion();

    if (Size < SYS_LOG_TIME_SIZE) {
        return 0;
    }

    memcpy(Buf, TimeStringRingBuffer[Version].SysLogTime, SYS_LOG_TIME_SIZE - 1);
    return SYS_LOG_TIME_SIZE - 1;
}

static int FetchTimeVersion(bool Force) {

    std::lock_guard<spin_lock> g(Lock);

    if (UpdateTimestamp || Force) {
        UpdateTimeString();
    }

    return TimestampVersion;
}

static void TimerHandle(int) {
    UpdateTimestamp = true;
}

static void InterruptHandle(int) {
    Interrupted = true;
}

static void UpdateTimeString() {

    uint64_t Time, OldTime;
    int Year, Month, MonthDay, Hour, Minute, Second, DayOfWeek, Days, Leap, DayOfYear;

    OldTime = TimeStringRingBuffer[TimestampVersion].Timestamp;

    gettimeofday(&Timestamp, nullptr);
    Time = (uint64_t) Timestamp.tv_sec;

    HighResolutionTimestamp = Time * U_SECOND_SIZE + (uint64_t) Timestamp.tv_usec;

    if (Time == OldTime) {
        return;
    }

    TimestampVersion = (TimestampVersion + 1) % NUM_TIME_SLOT;
    TimeStringRingBuffer[TimestampVersion].Timestamp = Time;

    Days = (int) (Time / 86400);
    Second = (int) (Time % 86400);

    if (Days > 2932896) {
        Days = 2932896;
        Second = 86399;
    }

    DayOfWeek = (4 + Days) % 7;
    Hour = Second / 3600;
    Second = Second % 3600;
    Minute = Second / 60;
    Second = Second % 60;

    Days = Days - (31 + 28) + 719527;
    Year = (Days + 2) * 400 / (365 * 400 + 100 - 4 + 1);
    DayOfYear = Days - (365 * Year + Year / 4 - Year / 100 + Year / 400);

    if (DayOfYear < 0) {
        Leap = (Year % 4 == 0) && (Year % 100 || (Year % 400 == 0));
        DayOfYear = 365 + Leap + DayOfYear;
        Year--;
    }
    Month = (DayOfYear + 31) * 10 / 306;

    MonthDay = DayOfYear - (367 * Month / 12 - 30) + 1;

    if (DayOfYear >= 306) {
        Year++;
        Month -= 10;
    } else {
        Month += 2;
    }

    sprintf(TimeStringRingBuffer[TimestampVersion].ErrorLogTime,
            "%4d/%02d/%02d %02d:%02d:%02d", Year, Month, MonthDay, Hour, Minute, Second);
    sprintf(TimeStringRingBuffer[TimestampVersion].HTTPTime,
            "%s, %02d %s %4d %02d:%02d:%02d GMT",
            WeekString[DayOfWeek], MonthDay, MonthString[Month - 1], Year, Hour, Minute, Second);
    sprintf(TimeStringRingBuffer[TimestampVersion].HTTPLogTime,
            "%02d/%s/%d:%02d:%02d:%02d %c%02i%02i",
            MonthDay, MonthString[Month - 1], Year, Hour, Minute, Second, '+', 8, 0);
    sprintf(TimeStringRingBuffer[TimestampVersion].HTTPLogTimeISO8601,
            "%4d-%02d-%02dT%02d:%02d:%02d%c%02i:%02i", Year, Month, MonthDay, Hour, Minute, Second, '+', 8, 0);
    sprintf(TimeStringRingBuffer[TimestampVersion].SysLogTime,
            "%s %2d %02d:%02d:%02d", MonthString[Month - 1], MonthDay, Hour, Minute, Second);

    UpdateTimestamp = false;
}

int ngx::Core::Support::ForceSleep(uint64_t NanoSeconds) {

    struct timespec ts;

    ts.tv_nsec = NanoSeconds % (NANO_SECOND_SIZE);
    ts.tv_sec = NanoSeconds / (NANO_SECOND_SIZE);

    while (-1 == nanosleep(&ts, &ts)) {
        if (errno != EINTR) {
            return errno;
        }
    }

    return 0;
}








