namespace ngx::Core {

    extern "C" {

        uint64_t GetTimeStamp();
        int WriteISOTime(u_char *PointeToBuffer, size_t Size);

    };
}